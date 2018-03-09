











/// <reference path="../knockout.js"/>






var cls_app_content_vm = function () {
    this.content = ko.observable();
};

var cls_app_menu_item_vm = function () {
    this.title = ko.observable();
    this.icon = ko.observable();
    this.uri = ko.observable();
};

var cls_app_main_menu_vm = function () {
    this.menus = ko.observableArray();
};

var func_app_init = function () {



};