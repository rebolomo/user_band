<?php
// $Id: sina_login.info,v 0.1 20110523 Felix $

function sina_login_page_login(){
	require_once("lib/weibooauth.php");
	$sina_config = sina_login_get_config();
	//	REBOL edit
	$o = new /*WeiboOAuth*/SaeTOAuthV2( $sina_config['app_key'] , $sina_config['app_secret']  );
	//$token = $o->getRequestToken();
	

	//if($token["oauth_token"])
	{
	    //$_SESSION['sina_request_token'] = $token;
	    //	REBOL edit
	    //$aurl = $o->getAuthorizeURL( $token['oauth_token'] ,false , $sina_config['return_url']);
			$aurl = $o->getAuthorizeURL( $sina_config['return_url'] );
		watchdog('band',$aurl);
	    header("Location:$aurl");
	}
	/*
	else
	{
	    //错误了,可以定义跳转到对应错误页面
	    drupal_set_message('新浪微博帐号验证失败', 'error');
	    drupal_goto('user');
	}*/
}

function sina_login_page_callback(){

	require_once("lib/weibooauth.php");
	$sina_config = sina_login_get_config();
	//	REBOL edit
	$o = new /*WeiboOAuth*/SaeTOAuthV2( $sina_config['app_key'] , $sina_config['app_secret']/* , $_SESSION['sina_request_token']['oauth_token'] , $_SESSION['sina_request_token']['oauth_token_secret'] */ );

	//	REBOL edit
	$keys = array();
	$keys['code'] = $_REQUEST['code'];
	$keys['redirect_uri'] = $sina_config['return_url'];
	$accessToken = $o->getAccessToken( 'code', $keys ) ;
	//$accessToken = $o->getAccessToken(  $_REQUEST['oauth_verifier'] ) ;
	
	$_SESSION['sina_access_token'] = $accessToken;
	//	REBOL edit
	if($accessToken["access_token"])//"oauth_token"])
	{
		//	REBOL edit
		//$oauth_token  = $accessToken["oauth_token"];
		//$oauth_secret = $accessToken["oauth_token_secret"];
		
		$c = new SaeTClientV2( $sina_config['app_key'] , $sina_config['app_secret'] , $_SESSION['sina_access_token']['access_token'] );
		//$ms  = $c->home_timeline(); // done
		$uid_get = $c->get_uid();
		$ouid = $uid_get['uid'];
		$user_message = $c->show_user_by_id( $ouid);//根据ID获取用户等基本信息
		$name = $user_message['screen_name'];
		//$c = new WeiboClient( $sina_config['app_key'] , $sina_config['app_secret'] , $oauth_token , $oauth_secret );
		//$userInfo = $c->verify_credentials();
		//$ouid = $userInfo['id'];  
		//$name = $userInfo['name'];
		//说明：新浪微博官方不提供Email
		$token = $_SESSION['sina_access_token']; 

		//执行用户绑定接口程序，参数 $ouser 结构见 user_band_bandapi 函数定义
		$ouser = array(
			'ouid'=> $ouid,
			'name'=> $name,
			'token' => $token,
			'from'=>'sina',
			);

		user_band_bandapi($ouser);
	}
	else
	{
	    //错误了,可以定义跳转到对应错误页面
	    drupal_set_message('新浪微博帐号验证失败', 'error');
	    drupal_goto('user');
	}
	
}




