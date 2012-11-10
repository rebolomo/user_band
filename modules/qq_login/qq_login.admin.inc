<?php
// $Id: qq_login.info,v 0.1 20110603 Felix $

function qq_login_admin_settings(){
  $form['qq_login_is_login'] = array(
    '#type' => 'radios',
    '#title' => '允许使用QQ帐号登录',
    '#default_value' => variable_get('qq_login_is_login', 1),
    '#options' => array('不允许', '允许'),
    '#description' => '将与现有账户绑定或创建一个新用户',
    '#required' => TRUE,
  );
  $form['qq_login_app_id'] = array(
    '#type' => 'textfield',
    '#title' => 'APP_KEY',
    '#default_value' => variable_get('qq_login_app_id', '100320495'),
    '#description' => '输入你申请到的app id',
    '#required' => TRUE,
  );
  $form['qq_login_app_key'] = array(
    '#type' => 'textfield',
    '#title' => 'APP_SECRET',
    '#default_value' => variable_get('qq_login_app_key', 'a6af621bf1aaa1e4286e650da355c3b1'),
    '#description' => '输入你申请到的app key',
    '#required' => TRUE,
  );
  return system_settings_form($form);
}