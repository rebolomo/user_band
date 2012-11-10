<?php
// $Id: user_band.pages.inc,v 1.5 2011/04/22 13:52:44 Felix Exp $

/*
 * 
 */
function user_band_email($ouser){
	//var_dump($ouser);
	$output = drupal_get_form('user_band_email_form',$ouser);
    //echo $output;
    return $output;
    //exit();
}
 
function user_band_email_form($form, &$form_state){
  $ouser = $form_state['build_info']['args'][0];
	$raw_user = unserialize(urldecode(base64_decode($ouser)));
	$form['user_band_login_name'] = array(
    '#type' => 'textfield',
    '#title' => 'Name',
    '#description' => '您可以编辑您的用户名，提交后则无法再次修改',
    '#required' => 1,
		'#default_value' => $raw_user['name'],
  );
	
  $form['user_band_login_email'] = array(
    '#type' => 'textfield',
    '#title' => 'Email',
    '#description' => '请填写一个您常用的邮箱地址以方便您的使用<br />请放心，我们和您一样讨厌垃圾邮件！',
    '#required' => 1
  );
	
  $form['ouser'] = array(
    '#type' => 'hidden',
    '#value' => $ouser,
  );
  $form['#validate'][] = 'user_band_email_form_validate';
  //$form['#redirect'] = "";
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => '确认'
  );
  return $form;
}

function user_band_email_form_validate($form, &$form_state) {
  $form_state['values']['user_band_login_email'] = trim($form_state['values']['user_band_login_email']);
  $email = $form_state['values']['user_band_login_email'];
	$name = $form_state['values']['user_band_login_name'];
  if (!valid_email_address($email)) {
    form_set_error('user_band_login_email', '请输入正确的Email格式');
  }
	$uid = user_band_get_uid_by_email($email);
	if( $uid > 0 )	{
		form_set_error('user_band_login_email', '该电子邮件地址已经被注册，请换一个。');
	}
	
	$uid = user_band_get_uid_by_name($name);
	if( $uid > 0 )	{
		form_set_error('user_band_login_name', '该用户名已注册，请换一个。');
	}
	
	//	REBOL todo, 在这里提示用户名字里面有非法字符，如果提交的时候还有，需要再提示
	//	REBOL todo, 加入js效果的提示，而不是提交表单后再报错
}

function user_band_email_form_submit($form, &$form_state){
	//	REBOL add
	//If new directory path does not exist, create it.
	$sina_path = file_default_scheme() . ':/'  . 'sinalog';
	file_prepare_directory( $sina_path, $mode = FILE_CREATE_DIRECTORY);
	//-----log for test login times cost---------------------------------------
	//$logtext = 'user_band_email-form_submit';
	//$filename = 'sinalog/'.time().'-18-'.$logtext.'.txt';
	//$dest= file_directory_path() .'/'. $filename;
	//file_save_data($logtext, $dest, FILE_EXISTS_RENAME);
	//-----End log-------------------------------------------------------------
	$email = trim($form_state['values']['user_band_login_email']);
	$name = trim($form_state['values']['user_band_login_name']);
	$uid = user_band_get_uid_by_email($email);
	$ouser = unserialize(urldecode(base64_decode($form_state['values']['ouser'])));
	if($uid == 0){ //$ouser中的Email尚未注册
		// TO DO: 用Email进行注册
		$ouser['email'] = $email;
		$ouser['name'] = $name;
	//-----log for test login times cost---------------------------------------
	//$logtext = 'user_band_email-user_band_reg';
	//$filename = 'sinalog/'.time().'-19-'.$logtext.'.txt';
	//$dest= file_directory_path() .'/'. $filename;
	//file_save_data($logtext, $dest, FILE_EXISTS_RENAME);
	//-----End log-------------------------------------------------------------
		$user = user_band_reg($ouser);
		$uid = $user->uid;
	}
	//-----log for test login times cost---------------------------------------
	//$logtext = 'user_band_email-user_band_band';
	//$filename = 'sinalog/'.time().'-20-'.$logtext.'.txt';
	//$dest= file_directory_path() .'/'. $filename;
	//file_save_data($logtext, $dest, FILE_EXISTS_RENAME);
	//-----End log-------------------------------------------------------------
	user_band_band($uid,$ouser);
	//$user_info = array('uid'=>$uid);
	//-----log for test login times cost---------------------------------------
	//$logtext = 'user_band_email-user_load';
	//$filename = 'sinalog/'.time().'-21-'.$logtext.'.txt';
	//$dest= file_directory_path() .'/'. $filename;
	//file_save_data($logtext, $dest, FILE_EXISTS_RENAME);
	//-----End log-------------------------------------------------------------
	$newuser = user_load($uid);
	//user_external_login($newuser); //为新用户登录
	$form_state['uid'] = $newuser->uid;
  user_login_submit(array(), $form_state);
	//-----log for test login times cost---------------------------------------
	//$logtext = 'user_band_email-jump to front';
	//$filename = 'sinalog/'.time().'-22-'.$logtext.'.txt';
	//$dest= file_directory_path() .'/'. $filename;
	//file_save_data($logtext, $dest, FILE_EXISTS_RENAME);
	//-----End log-------------------------------------------------------------
	drupal_goto(); // jump to <front>
}
