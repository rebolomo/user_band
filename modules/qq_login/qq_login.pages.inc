<?php
// $Id: qq_login.info,v 0.1 20110603 Felix $

function qq_login_page_login(){
	require_once("lib/utils.php");
	$qq_config = qq_login_get_config();
	$appid = $qq_config['app_id']; 
	$appkey = $qq_config['app_key'];
	$callback = $qq_config['return_url'];
    //跳转到QQ登录页的接口地址, 不要更改!!
    $redirect = "http://openapi.qzone.qq.com/oauth/qzoneoauth_authorize?oauth_consumer_key=$appid&";

    //调用get_request_token接口获取未授权的临时token
    $result = array();
    $request_token = get_request_token($appid, $appkey);
    parse_str($request_token, $result);

    //request token, request token secret 需要保存起来
    //在demo演示中，直接保存在全局变量中.
    //为避免网站存在多个子域名或同一个主域名不同服务器造成的session无法共享问题
    //请开发者按照本SDK中comm/session.php中的注释对session.php进行必要的修改，以解决上述2个问题，
    $_SESSION["qq_token"]        = $result["oauth_token"];
    $_SESSION["qq_secret"]       = $result["oauth_token_secret"];

    if ($result["oauth_token"] == "")
    {
        //错误了,可以定义跳转到对应错误页面
	    drupal_set_message('QQ帐号验证失败', 'error');
	    drupal_goto('user');
    }

    ////构造请求URL
    $redirect .= "oauth_token=".$result["oauth_token"]."&oauth_callback=".rawurlencode($callback);
    header("Location:$redirect");
}

function qq_login_page_callback(){
	require_once("lib/utils.php");
	$qq_config = qq_login_get_config();
	$appid = $qq_config['app_id']; 
	$appkey = $qq_config['app_key'];	
	
	//用户使用QQ登录，并授权成功后，会返回用户的openid。此时需要检查返回的openid是否是合法id
	//我们不建议开发者使用该openid，而是使用获取access token之后返回的openid。
	if (!is_valid_openid($_REQUEST["openid"], $_REQUEST["timestamp"], $_REQUEST["oauth_signature"]))
	{
	    //错误了,可以定义跳转到对应错误页面
	    drupal_set_message('QQ帐号验证失败', 'error');
	    drupal_goto('user');
	}
	
	//用授权的request token换取access token
	$access_str = get_access_token($appid, $appkey, $_REQUEST["oauth_token"], $_SESSION["qq_secret"], $_REQUEST["oauth_vericode"]);
	//echo "access_str:$access_str\n";
	$result = array();
	parse_str($access_str, $result);
	
	//print_r($result);
	
	//错误处理
	if (isset($result["error_code"]))
	{
	    //错误了,可以定义跳转到对应错误页面
	    drupal_set_message('QQ帐号验证失败', 'error');
	    drupal_goto('user');
	}
	
	
	//将access token，openid保存起来！！！
	//在demo演示中，直接保存在全局变量中.
	//为避免网站存在多个子域名或同一个主域名不同服务器造成的session无法共享问题
	//请开发者按照本SDK中comm/session.php中的注释对session.php进行必要的修改，以解决上述2个问题，
	$_SESSION["qq_token"]   = $result["oauth_token"];
	$_SESSION["qq_secret"]  = $result["oauth_token_secret"]; 
	$_SESSION["qq_openid"]  = $result["openid"];

	//获取用户信息
	$arr = get_user_info($appid, $appkey, $_SESSION["qq_token"], $_SESSION["qq_secret"], $_SESSION["qq_openid"]);
	//第三方处理用户绑定逻辑
	//将openid与第三方的帐号做关联
	$ouser = array(
		'ouid'=> $result['openid'],
		'name'=> $arr['nickname'],
		'token' => array('oauth_token'=>$result['oauth_token'],'oauth_token_secret'=>$result['oauth_token_secret']),
		'from'=>'qq',
		);
	
	user_band_bandapi($ouser);
}

 /**
 * @brief 请求临时token.请求需经过URL编码，编码时请遵循 RFC 1738
 *  
 * @param $appid
 * @param $appkey
 *
 * @return 返回字符串格式为：oauth_token=xxx&oauth_token_secret=xxx
 */
function get_request_token($appid, $appkey)
{
    //请求临时token的接口地址, 不要更改!!
    $url    = "http://openapi.qzone.qq.com/oauth/qzoneoauth_request_token?";

    //生成oauth_signature签名值。签名值生成方法详见（http://wiki.opensns.qq.com/wiki/【QQ登录】签名参数oauth_signature的说明）
    //（1） 构造生成签名值的源串（HTTP请求方式 & urlencode(uri) & urlencode(a=x&b=y&...)）
	$sigstr = "GET"."&".rawurlencode("http://openapi.qzone.qq.com/oauth/qzoneoauth_request_token")."&";

	//必要参数
    $params = array();
    $params["oauth_version"]          = "1.0";
    $params["oauth_signature_method"] = "HMAC-SHA1";
    $params["oauth_timestamp"]        = time();
    $params["oauth_nonce"]            = mt_rand();
    $params["oauth_consumer_key"]     = $appid;

    //对参数按照字母升序做序列化
    $normalized_str = get_normalized_string($params);
    $sigstr        .= rawurlencode($normalized_str);

	//（2）构造密钥
    $key = $appkey."&";

 	//（3）生成oauth_signature签名值。这里需要确保PHP版本支持hash_hmac函数
    $signature = get_signature($sigstr, $key);
	
	//构造请求url
    $url      .= $normalized_str."&"."oauth_signature=".rawurlencode($signature);

    return file_get_contents($url);
}

/**
 * @brief 获取access_token。请求需经过URL编码，编码时请遵循 RFC 1738
 *
 * @param $appid
 * @param $appkey
 * @param $request_token
 * @param $request_token_secret
 * @param $vericode
 *
 * @return 返回字符串格式为：oauth_token=xxx&oauth_token_secret=xxx&openid=xxx&oauth_signature=xxx&oauth_vericode=xxx&timestamp=xxx
 */
function get_access_token($appid, $appkey, $request_token, $request_token_secret, $vericode)
{
    //请求具有Qzone访问权限的access_token的接口地址, 不要更改!!
    $url    = "http://openapi.qzone.qq.com/oauth/qzoneoauth_access_token?";
   
    //生成oauth_signature签名值。签名值生成方法详见（http://wiki.opensns.qq.com/wiki/【QQ登录】签名参数oauth_signature的说明）
    //（1） 构造生成签名值的源串（HTTP请求方式 & urlencode(uri) & urlencode(a=x&b=y&...)）
	$sigstr = "GET"."&".rawurlencode("http://openapi.qzone.qq.com/oauth/qzoneoauth_access_token")."&";

    //必要参数，不要随便更改!!
    $params = array();
    $params["oauth_version"]          = "1.0";
    $params["oauth_signature_method"] = "HMAC-SHA1";
    $params["oauth_timestamp"]        = time();
    $params["oauth_nonce"]            = mt_rand();
    $params["oauth_consumer_key"]     = $appid;
    $params["oauth_token"]            = $request_token;
    $params["oauth_vericode"]         = $vericode;

    //对参数按照字母升序做序列化
    $normalized_str = get_normalized_string($params);
    $sigstr        .= rawurlencode($normalized_str);

    //echo "sigstr = $sigstr";

	//（2）构造密钥
    $key = $appkey."&".$request_token_secret;

	//（3）生成oauth_signature签名值。这里需要确保PHP版本支持hash_hmac函数
    $signature = get_signature($sigstr, $key);

	//构造请求url
    $url      .= $normalized_str."&"."oauth_signature=".rawurlencode($signature);

    return file_get_contents($url);
}

 /*
 * @brief 获取用户信息.请求需经过URL编码，编码时请遵循 RFC 1738
 * 
 * @param $appid
 * @param $appkey
 * @param $access_token
 * @param $access_token_secret
 * @param $openid
 *
 */
function get_user_info($appid, $appkey, $access_token, $access_token_secret, $openid)
{
	//获取用户信息的接口地址, 不要更改!!
    $url    = "http://openapi.qzone.qq.com/user/get_user_info";
    $info   = do_get($url, $appid, $appkey, $access_token, $access_token_secret, $openid);
    $arr = array();
    $arr = json_decode($info, true);

    return $arr;
}
