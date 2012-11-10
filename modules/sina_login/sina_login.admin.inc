<?php
// $Id: sina_login.info,v 0.1 20110523 Felix $

function sina_login_admin_settings(){
  $form['sina_login_is_login'] = array(
    '#type' => 'radios',
    '#title' => '允许使用新浪微博帐号登录',
    '#default_value' => variable_get('sina_login_is_login', 1),
    '#options' => array('不允许', '允许'),
    '#description' => '将与现有账户绑定或创建一个新用户',
    '#required' => TRUE,
  );
  $form['sina_login_app_key'] = array(
    '#type' => 'textfield',
    '#title' => 'APP_KEY',
    '#default_value' => variable_get('sina_login_app_key', '241329458'),
    '#description' => '输入你申请到的key',
    '#required' => TRUE,
  );
  $form['sina_login_app_secret'] = array(
    '#type' => 'textfield',
    '#title' => 'APP_SECRET',
    '#default_value' => variable_get('sina_login_app_secret', '8e17a3d7eeae7c5bb3a9f488f3793a75'),
    '#description' => '输入你申请到的Secret',
    '#required' => TRUE,
  );
  $form['sina_login_post'] = array(
    '#type' => 'select',
    '#title' => '自动发送微博？',
    '#options' => array('0' => '否', '1' => '是'),
    '#default_value' => variable_get('sina_login_post', '1'),
    '#description' => '登录时是否自动发送微博',
    '#required' => TRUE,
  );
  $form['sina_login_post_text'] = array(
    '#type' => 'textarea',
    '#title' => '微博内容',
    '#default_value' => variable_get('sina_login_post_text', ''),
    '#description' => '自动发送微博的内容',
    '#required' => FALSE,
  );
  return system_settings_form($form);
}