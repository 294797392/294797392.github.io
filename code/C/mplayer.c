#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WINDOWS
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
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
		fprintf(stdout, "create process fail:%u\n", GetLastError());
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

int mpops_process_is_exit(mplayer_t *mp)
{
	return mp->priv->pinfo.hProcess == NULL;
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
	char command[256] = { '\0' };
	strncpy(command, cmd, sizeof(command));
	strncat(command, "\r\n", 2);

	DWORD dwWritten;
	if (WriteFile(mp->priv->hWrite, command, strlen(command), &dwWritten, NULL) == FALSE)
	{
		fprintf(stdout, "send %s error, last error:%u\n", cmd, GetLastError());
		return MP_SEND_COMMAND_FAILED;
	}
	else
	{
		return MP_SUCCESS;
	}
}

int mpops_read_data(mplayer_t *mp, char *buff, int size)
{
	DWORD dwRead;
	if (ReadFile(mp->priv->hRead, buff, size, &dwRead, NULL) == FALSE)
	{
		fprintf(stdout, "read data error, last error:%u\n", GetLastError());
		return MP_READ_DATA_FAILED;
	}
	else
	{
		return MP_SUCCESS;
	}
}

#else

struct tagMPLAYER_PRIV {
	int fd_write;
	int fd_read;
	pid_t pid;
};

/* 打开mplayer播放进程并开始播放 */
int mpops_open_player_process(mplayer_t *mp)
{
	int pipe1[2], pipe2[2];
	if(pipe(pipe1) < 0 || pipe(pipe2) < 0)
	{
		fprintf(stdout, "create pipe failed, errno:%d\n", errno);
		return MP_CREATE_PROCESS_FAILED;
	}

	pid_t pid = fork();
	if (pid > 0)
	{
		mp->priv->fd_read = pipe1[0];
		close(pipe1[1]);
		mp->priv->fd_write = pipe2[1];
		close(pipe2[0]);
		mp->priv->pid = pid;
	}
	else if(pid == 0)
	{
		/* mplayer process */
		close(pipe1[0]);
		close(pipe2[1]);
		dup2(pipe1[1], STDOUT_FILENO);
		dup2(pipe2[0], STDIN_FILENO);
		char *argv[5] = { MPLAYER_PATH, "-slave", "-quiet", mp->source, NULL };
		if(execvp(MPLAYER_PATH, argv) == -1)
		{
			fprintf(stdout, "load mplayer image failed, errno:%d\n", errno);
			return MP_CREATE_PROCESS_FAILED;
		}
	}
	else if(pid == -1)
	{
		fprintf(stdout, "fork mplayer process failed, errno:%d\n", errno);
		return MP_CREATE_PROCESS_FAILED;
	}

	return MP_SUCCESS;
}

int mpops_process_is_exit(mplayer_t *mp)
{
	return mp->priv->pid == 0;
}

/* 等待mplayer播放进程结束 */
int mpops_wait_process_exit(mplayer_t *mp)
{
	int exit_status;
	if(waitpid(mp->priv->pid, &exit_status, 0) < 0)
	{
		if(errno == EINTR)
		{
			fprintf(stdout, "receive EINTR signal\n");
		}
		else
		{
			fprintf(stdout, "waitpid failed, errno:%d\n", errno);
		}
	}
	return 1;
}

/* 关闭进程 */
void mpops_close_player_process(mplayer_t *mp)
{
	if(kill(mp->priv->pid, SIGKILL) == -1)
	{
		fprintf(stdout, "kill mplayer process failed, errno:%d\n", errno);
	}
}

/* 释放mplayer进程所占用的资源 */
void mpops_release_process_resource(mplayer_t *mp)
{
	close(mp->priv->fd_read);
	close(mp->priv->fd_write);
	mp->priv->fd_read = 0;
	mp->priv->fd_write = 0;
	mp->priv->pid = 0;
}

/* 向mplayer播放进程发送消息 */
int mpops_send_command(mplayer_t *mp, const char *cmd, int size)
{
	char command[256] = { '\0' };
	strncpy(command, cmd, sizeof(command));
	strncat(command, "\n", 1);

	if(write(mp->priv->fd_write, command, strlen(command)) == -1)
	{
		fprintf(stdout, "send %s error, errno:%d\n", cmd, errno);
		return MP_SEND_COMMAND_FAILED;
	}
	else
	{
		return MP_SUCCESS;
	}
}

/* 从mplayer进程读取数据 */
int mpops_read_data(mplayer_t *mp, char *buff, int size)
{
	if(read(mp->priv->fd_read, buff, (size_t)size) < 0)
	{
		fprintf(stdout, "read data error, errno:%d\n", errno);
		return MP_READ_DATA_FAILED;
	}
	else
	{
		return MP_SUCCESS;
	}
}

#endif

static mplayer_ops_t ops_instance =
{
	.mpops_open_player_process = mpops_open_player_process,
	.mpops_close_player_process = mpops_close_player_process,
	.mpops_wait_process_exit = mpops_wait_process_exit,
	.mpops_process_is_exit = mpops_process_is_exit,
	.mpops_release_process_resource = mpops_release_process_resource,
	.mpops_send_command = mpops_send_command,
	.mpops_read_data = mpops_read_data
};


static BOOL parse_key_value_pair(const char *kvpair, char splitter, char *key, int key_size, char *value, int value_max_size)
{
	int cnt = strlen(kvpair);
	for (int i = 0; i < cnt; i++)
	{
		if (kvpair[i] == splitter)
		{
			if (i > key_size || cnt - i > value_max_size)
			{
				return FALSE;
			}
			strncpy(key, kvpair, i);
			strncpy(value, kvpair + i + 1, cnt - i);
			break;
		}
	}
	return TRUE;
}

void* mplayer_monitor_thread_process(void* userdata)
{
	mplayer_t *mp = (mplayer_t*)userdata;
	fprintf(stdout, "wait mplayer process stopped..\n");
	mp->ops->mpops_wait_process_exit(mp);
	fprintf(stdout, "mplayer process stopped\n");
	mp->ops->mpops_release_process_resource(mp);
	mp->status = MPSTAT_STOPPED;
	if (mp->listener && mp->listener->handler)
	{
		int status = (int)MPSTAT_STOPPED;
		mp->listener->handler(MPEVT_STATUS_CHANGED, &status, mp->listener->userdata);
	}
	return NULL;
}

int mplayer_read(mplayer_t *mp, const char *substr, char *buff, int size)
{
	char line[1024] = { '\0' };
	while (1)
	{
		char c[1] = { '\0' };
		int ret = mp->ops->mpops_read_data(mp, c, 1);
		if (ret != MP_SUCCESS)
		{
			return ret;
		}
		if (c[0] == '\n')
		{
			if (strstr(line, substr))
			{
				strncpy(buff, line, size);
				return MP_SUCCESS;
			}

			memset(line, 0, sizeof(line));
			continue;
		}

		if (c[0] != '\r')
		{
			strncat(line, c, 1);
		}
	}
}

void mplayer_retrive_media_info(mplayer_t *mp, const char *retrive_cmd, const char *info_key, char *value_buf, int buf_size)
{
	if (mp->ops->mpops_send_command(mp, retrive_cmd, strlen(retrive_cmd)) != MP_SUCCESS)
	{
		return;
	}

	char result[1024] = { '\0' };
	if (mplayer_read(mp, info_key, result, sizeof(result)) != MP_SUCCESS)
	{
		return;
	}

	char key[128] = { '\0' };
	parse_key_value_pair(result, '=', key, sizeof(key), value_buf, buf_size);
}

mplayer_t* mplayer_create_instance()
{
	mplayer_t *instance = (mplayer_t*)malloc(sizeof(mplayer_t));
	memset(instance, 0, sizeof(mplayer_t));
	instance->ops = &ops_instance;
	instance->volume = VOLUME_DEFAULT;
	instance->status = MPSTAT_STOPPED;

	mplayer_priv_t *priv = (mplayer_priv_t*)malloc(sizeof(mplayer_priv_t));
	memset(priv, 0, sizeof(mplayer_priv_t));
	instance->priv = priv;

	return instance;
}

void mplayer_free_instance(mplayer_t *mp)
{
	mp->ops->mpops_close_player_process(mp);
	mp->ops->mpops_release_process_resource(mp);
	free(mp);
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
	if (!mp->ops->mpops_process_is_exit(mp))
	{
		char cmd[1024] = { '\0' };
		snprintf(cmd, sizeof(cmd), "loadfile %s", mp->source);
		return mp->ops->mpops_send_command(mp, cmd, strlen(cmd));
	}

	int ret = mp->ops->mpops_open_player_process(mp);
	if (ret != MP_SUCCESS)
	{
		return ret;
	}

	pthread_create(&mp->monitor_thread, NULL, mplayer_monitor_thread_process, mp);
	mp->status = MPSTAT_PLAYING;
	return MP_SUCCESS;
}

void mplayer_stop(mplayer_t *mp)
{
	mp->ops->mpops_close_player_process(mp);
	mp->ops->mpops_release_process_resource(mp);
	mp->status = MPSTAT_STOPPED;
}

int mplayer_pause(mplayer_t *mp)
{
	const char *pause_cmd = "pause";
	int ret = mp->ops->mpops_send_command(mp, pause_cmd, strlen(pause_cmd));
	if (ret == MP_SUCCESS)
	{
		mp->status = MPSTAT_PAUSED;
	}
	return ret;
}

int mplayer_resume(mplayer_t *mp)
{
	const char *resume_cmd = "pause";
	int ret = mp->ops->mpops_send_command(mp, resume_cmd, strlen(resume_cmd));
	if (ret == MP_SUCCESS)
	{
		mp->status = MPSTAT_PLAYING;
	}
	return ret;
}

void mplayer_increase_volume(mplayer_t *mp)
{
	if (mp->volume < VOLUME_MAX) {
		mp->volume += VOLUME_DELTA;

		char cmd[128] = { '\0' };
		snprintf(cmd, sizeof(cmd), "volume %i 1", mp->volume);
		mp->ops->mpops_send_command(mp, cmd, strlen(cmd));
	}
}

void mplayer_decrease_volume(mplayer_t *mp)
{
	if (mp->volume > VOLUME_MIN) {
		mp->volume -= VOLUME_DELTA;

		char cmd[128] = { '\0' };
		snprintf(cmd, sizeof(cmd), "volume %i 1", mp->volume);
		mp->ops->mpops_send_command(mp, cmd, strlen(cmd));
	}
}

int mplayer_get_duration(mplayer_t *mp)
{
	char duration[128] = { '\0' };
	mplayer_retrive_media_info(mp, "get_time_length", "ANS_LENGTH", duration, sizeof(duration));
	return strlen(duration) == 0 ? -1 : atoi(duration);
}

int mplayer_get_position(mplayer_t *mp)
{
	char position[128] = { '\0' };
	mplayer_retrive_media_info(mp, "get_time_pos", "ANS_TIME_POSITION", position, sizeof(position));
	return strlen(position) == 0 ? -1 : atoi(position);
}

void mplayer_listen_event(mplayer_t *mp, mplayer_listener_t listener)
{
	if (!mp->listener)
	{
		mp->listener = (mplayer_listener_t*)malloc(sizeof(mplayer_listener_t));
	}
	mp->listener->handler = listener.handler;
	mp->listener->userdata = listener.userdata;
}

mplayer_status_enum mplayer_get_status(mplayer_t *mp)
{
	return mp->status;
}

int mplayer_send_command(mplayer_t *mp, const char *cmd, int size)
{
	return mp->ops->mpops_send_command(mp, cmd, size);
}