

/// <reference path="jquery.js"/>


var database_url = "http://localhost:8888/";

var database = {
    get_menu_list: function (callback) {
        $.ajax({
            url: database_url + "svc/menu.json",
            success: function (data, textStatus, jqXHR) {
                if (callback) {
                    //var menus = JSON.parse(data);
                    callback(data);
                }
            }
        });
    }
};