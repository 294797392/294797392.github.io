











/// <reference path="../knockout.js"/>
/// <reference path="../database.js"/>






var app_menu_item_vm = function () {
    this.title = ko.observable();
    this.icon = ko.observable();
    this.uri = ko.observable();
};

var app_main_menu_vm = function () {
    this.menus = ko.observableArray();
};

var app_init = function () {

    

    // 定义变量
    var menu_item_vm = new app_main_menu_vm();
    var callback_get_menu_list = function (menus) {
        var len = menus.length;
        for (var i = 0; i < len; i++) {
            var menu = menus[i];
            var menu_item = new app_menu_item_vm();
            menu_item.title(menu.title);
            menu_item_vm.menus.push(menu_item);
        }
    }

    // 绑定ViewModel
    ko.applyBindings({ menus: menu_item_vm.menus });

    // 获取读取
    database.get_menu_list(callback_get_menu_list);
};