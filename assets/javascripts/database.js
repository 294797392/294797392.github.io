/// <reference path="jquery.js"/>

//var database_url = "http://localhost:8888/";
var svc_root_url = "https://294797392.github.io/svc/"

function doQuery(url, callback)
{
    $.ajax({
        url: svc_root_url + url,
        success: function (data, textStatus, jqXHR) {
            if (callback) {
                //var menus = JSON.parse(data);
                callback(data);
            }
        }
    });
}

var database = {
    queryMainMenus: function (callback) {
        doQuery("mainMenu.json", callback);
    },

    queryBlogMenus: function (callback) {
        doQuery("blogMenu.json", callback)
    }
};