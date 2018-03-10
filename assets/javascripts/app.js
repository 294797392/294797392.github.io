














/// <reference path="knockout.js"/>
/// <reference path="database.js"/>





var blogInit = function () {

    // 定义变量
    var menuList = ko.observableArray();

    // 绑定界面
    $('.ui.accordion').accordion({ "exclusive": false });
    //$('.button').popup();
    
    ko.applyBindings({ menus: menuList }, document.getElementById("blog_menu"));

    // 初始化数据
    database.queryBlogMenus(function (menus) {
        var length = menus.length;
        for (var i = 0; i < length; i++) {
            var menu = menus[i];
            menuList.push(menu);
        }
    });
};

var appInit = function () {

    // 定义变量
    var menuList = ko.observableArray();

    // 绑定界面
    ko.applyBindings({ menus: menuList }, document.getElementById("main_menu"));

    // 初始化数据
    database.queryMainMenus(function (menus) {
        var len = menus.length;
        for (var i = 0; i < len; i++) {
            var menu = menus[i];
            menuList.push(menu);
        }
    });
};