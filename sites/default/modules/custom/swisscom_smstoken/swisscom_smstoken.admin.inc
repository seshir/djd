<?php
/**
 * @file
 * Administration Settings for Swisscom SMS Token module.
 */

/**
 * Form builder; Configure site settings.
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function swisscom_smstoken_admin_settings() {
  $form = array();

  $form['description']['#markup'] = t('This page contains custom settings for Swisscom SMS Validation.');

  $form['sms_token'] = array(
    '#type' => 'fieldset',
    '#title' => t('SMS Token Endpoint Settings'),
  );

  $form['sms_token']['swisscom_smstoken_endpoint'] = array(
    '#type'          => 'textfield',
    '#title'         => 'SMS Token Endpoint',
    '#description'   => t('Swisscom SMS Token Endpoint URL, no trailing slash.'),
    '#default_value' => variable_get('swisscom_smstoken_endpoint', FALSE),
    '#required'      => TRUE,
  );

  $form['sms_token']['swisscom_smstoken_apikey'] = array(
    '#type'          => 'textfield',
    '#title'         => 'SMS Token API Key',
    '#description'   => t('Swisscom SMS Token API Key.'),
    '#default_value' => variable_get('swisscom_smstoken_apikey', FALSE),
    '#required'      => TRUE,
  );
  
  $form['sms_token']['swisscom_smstoken_expiretime'] = array(
    '#type'          => 'textfield',
    '#title'         => 'SMS Token Expiry Time',
    '#description'   => t('Swisscom SMS Token Expiry Time.'),
    '#default_value' => variable_get('swisscom_smstoken_expiretime', FALSE),
    '#required'      => TRUE,
  );
  
  $form['sms_token']['swisscom_smstoken_message'] = array(
    '#type'          => 'textarea',
    '#title'         => 'SMS Token Message',
    '#description'   => t('Swisscom SMS Token message.e.g "Please enter the following token: %TOKEN% ". The placeholder %TOKEN% will be replaced by backend service.'),
    '#default_value' => variable_get('swisscom_smstoken_message', FALSE),
    '#required'      => TRUE,	
  );
  $form['sms_token']['swisscom_smstoken_eventcorrelationid'] = array(
    '#type'          => 'textfield',
    '#title'         => 'SMS Event Correlation ID',
    '#description'   => t('Swisscom SMS Event Correlation ID for backend service.'),
    '#default_value' => variable_get('swisscom_smstoken_eventcorrelationid', 'SnapTest'),
  );
  $form['sms_token']['swisscom_smstoken_host'] = array(
    '#type'          => 'textfield',
    '#title'         => 'HTTP header: "Host"',
    '#description'   => t('HTTP header host to be sent for backend verifications.e.g swisscom.apigee.net '),
    '#default_value' => variable_get('swisscom_smstoken_host','swisscom.apigee.net'),
  );
  
  return system_settings_form($form);
}
