#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WINDOWS
#include <windows.h>
#else
#endif

#include "mplayer.h"

#ifdef WINDOWS

struct tagMPLAYER_PRIV
{
	HANDLE hWrite;
	HANDLE hRead;
	PROCESS_INFORMATION pinfo;
};

int mpops_open_player_process(mplayer_t *mp)
{
#pragma region Create Pipe

	HANDLE pipe1[2];
	HANDLE pipe2[2];
	SECURITY_ATTRIBUTES saAttr;
	// Set the bInheritHandle flag so pipe handles are inherited. 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;
	// Create a pipe for the child process's STDOUT. 
	if (!CreatePipe(&pipe1[0], &pipe1[1], &saAttr, 0))
	{
		goto freepipe;
	}
	// Ensure the read handle to the pipe for STDOUT is not inherited.
	if (!SetHandleInformation(pipe1[0], HANDLE_FLAG_INHERIT, 0))
	{
		goto freepipe;
	}
	// Create a pipe for the child process's STDIN. 
	if (!CreatePipe(&pipe2[0], &pipe2[1], &saAttr, 0))
	{
		goto freepipe;
	}
	// Ensure the write handle to the pipe for STDIN is not inherited. 
	if (!SetHandleInformation(pipe2[1], HANDLE_FLAG_INHERIT, 0))
	{
		goto freepipe;
	}

#pragma endregion

#pragma region Create Process

	STARTUPINFO siStartInfo = { 0 };
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdOutput = pipe1[1];
	siStartInfo.hStdInput = pipe2[0];
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
	char command[512] = { '\0' };
	snprintf(command, sizeof(command), "-quiet -slave %s", mp->source);
	PROCESS_INFORMATION piProcInfo = { 0 };
	if (!CreateProcess(MPLAYER_PATH,
		command,		            // command line
		NULL,               // process security attributes 
		NULL,               // primary thread security attributes 
		TRUE,               // handles are inherited 
		0,                  // creation flags 
		NULL,               // use parent's environment 
		NULL,               // use parent's current directory 
		&siStartInfo,       // STARTUPINFO pointer 
		&piProcInfo))       // receives PROCESS_INFORMATION
	{
		fprintf(stdout, "create process fail:%u", GetLastError());
		goto freepipe;
	}

#pragma endregion

	memcpy(&mp->priv->pinfo, &piProcInfo, sizeof(PROCESS_INFORMATION));
	mp->priv->hWrite = pipe2[1];
	mp->priv->hRead = pipe1[0];
	return MP_SUCCESS;

freepipe:
	MP_SAFE_FREE(CloseHandle, &pipe1[0]);
	MP_SAFE_FREE(CloseHandle, &pipe1[1]);
	MP_SAFE_FREE(CloseHandle, &pipe2[0]);
	MP_SAFE_FREE(CloseHandle, &pipe2[1]);
	return MP_CREATE_PROCESS_FAILED;
}

int mpops_wait_process_exit(mplayer_t *mp)
{
	return WaitForSingleObject(mp->priv->pinfo.hProcess, INFINITE);
}

void mpops_close_player_process(mplayer_t *mp)
{
	if (mp->priv->pinfo.hProcess)
	{
		SetEvent(mp->priv->pinfo.hProcess);
	}
}

void mpops_release_process_resource(mplayer_t *mp)
{
	MP_SAFE_FREE(CloseHandle, &mp->priv->pinfo.hThread);
	MP_SAFE_FREE(CloseHandle, &mp->priv->pinfo.hProcess);
	MP_SAFE_FREE(CloseHandle, &mp->priv->hRead);
	MP_SAFE_FREE(CloseHandle, &mp->priv->hWrite);
}

int mpops_send_command(mplayer_t *mp, const char *cmd, int size)
{
	char command[64] = { '\0' };
	strncpy(command, cmd, sizeof(command));
	strncat(command, "\r\n", 2);

	DWORD dwWritten;
	if (WriteFile(mp->priv->hWrite, command, strlen(command), &dwWritten, NULL) == FALSE)
	{
		fprintf(stdout, "send %s error, last error:%u", cmd, GetLastError());
		return MP_SEND_COMMAND_FAILED;
	}
	else
	{
		return MP_SUCCESS;
	}
}

#else
struct tagMHANDLE {};
#endif

static mplayer_ops_t ops_instance =
{
	.mpops_open_player_process = mpops_open_player_process,
	.mpops_wait_process_exit = mpops_wait_process_exit,
	.mpops_close_player_process = mpops_close_player_process,
	.mpops_release_process_resource = mpops_release_process_resource,
	.mpops_send_command = mpops_send_command
};

void* mplayer_monitor_thread_process(void* userdata)
{
	mplayer_t *mp = (mplayer_t*)userdata;
	fprintf(stdout, "wait mplayer process stopped..\n");
	mp->ops->mpops_wait_process_exit(mp);
	fprintf(stdout, "mplayer process stopped\n");
	mp->ops->mpops_release_process_resource(mp);
	if (mp->event_listener)
	{
		mp->event_listener(MPEVT_STATUS_CHANGED, MPSTAT_STOPPED);
	}
	return NULL;
}

mplayer_t* mplayer_create_instance()
{
	mplayer_t *instance = (mplayer_t*)malloc(sizeof(mplayer_t));
	memset(instance, 0, sizeof(mplayer_t));
	instance->ops = &ops_instance;
	instance->volume = VOLUME_DEFAULT;

	mplayer_priv_t *priv = (mplayer_priv_t*)malloc(sizeof(mplayer_priv_t));
	memset(priv, 0, sizeof(mplayer_priv_t));
	instance->priv = priv;

	return instance;
}

void mplayer_open(mplayer_t *mp, const char *source, int source_size)
{
	strncpy(mp->source, source, source_size);
}

void mplayer_close(mplayer_t *mp)
{
	memset(mp->source, 0, sizeof(mp->source));
}

int mplayer_play(mplayer_t *mp)
{
	int ret = mp->ops->mpops_open_player_process(mp);
	if (ret != MP_SUCCESS)
	{
		return ret;
	}

	pthread_create(&mp->monitor_thread, NULL, mplayer_monitor_thread_process, mp);
	return MP_SUCCESS;
}

void mplayer_stop(mplayer_t *mp)
{
	mp->ops->mpops_close_player_process(mp);
	mp->ops->mpops_release_process_resource(mp);
}

int mplayer_pause(mplayer_t *mp)
{
	const char *pause_cmd = "pause";
	return mp->ops->mpops_send_command(mp, pause_cmd, strlen(pause_cmd));
}

int mplayer_resume(mplayer_t *mp)
{
	const char *resume_cmd = "pause";
	return mp->ops->mpops_send_command(mp, resume_cmd, strlen(resume_cmd));
}

void mplayer_increase_volume(mplayer_t *mp)
{
	if (mp->volume < VOLUME_MAX) {
		mp->volume += VOLUME_DELTA;

		char cmd[128] = { '\0' };
		snprintf(cmd, sizeof(cmd), "volume %i 1", mp->volume);
		return mp->ops->mpops_send_command(mp, cmd, strlen(cmd));
	}
}

void mplayer_decrease_volume(mplayer_t *mp)
{
	if (mp->volume > VOLUME_MIN) {
		mp->volume -= VOLUME_DELTA;

		char cmd[128] = { '\0' };
		snprintf(cmd, sizeof(cmd), "volume %i 1", mp->volume);
		return mp->ops->mpops_send_command(mp, cmd, strlen(cmd));
	}
}

void mplayer_listen_event(mplayer_t *mp, mp_event_listener listener)
{
	mp->event_listener = listener;
}