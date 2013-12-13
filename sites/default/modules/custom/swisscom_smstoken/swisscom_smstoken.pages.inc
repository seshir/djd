<?php

/**
 * @file
 * Page callbacks for the Swisscom SMS Token module.
 */

/**
 * Form constructor for the SMS verify form.
 */
function swisscom_smstoken_verify_form($form, &$form_state) {
  global $user;

  drupal_set_title(t('Verify Your Mobile Number'));

  // Add js that removes errors that are put on screen when submit is not ajax.
  $form['#attached']['js'] = array(
    drupal_get_path('module', 'swisscom_smstoken') . '/swisscom_smstoken.js',
  );

  // Provide a wrapper around the entire form, since we'll replace the whole
  // thing with each submit.
  $form['#prefix'] = '<div id="wizard-form-wrapper">';
  $form['#suffix'] = '</div>';
  $form['#tree'] = TRUE; // We want to deal with hierarchical form values.

  $form['description']['#markup'] = t('<p></p>');
  $form['description'] = array(
    '#markup' => '<p>' . t('Before you can create or manage your applications, you need to validate your mobile phone number via SMS. ' .
        'After submitting your number we will send you a verification code via SMS.') . '</p>',
  );

  // $form_state['storage'] has no specific drupal meaning, but it is
  // traditional to keep variables for multistep forms there.
  $step = empty($form_state['storage']['step']) ? 1 : $form_state['storage']['step'];
  $form_state['storage']['step'] = $step;

  switch ($step) {
    case 1:
      $form['step1'] = array(
        '#type' => 'fieldset',
        '#title' => t('Step 1 of 2: Enter mobile number'),
      );

      // Get mobile from the user profile.
      $account = user_load($user->uid);
      // Attach the Mobile Phone field from the user profile
      // TODO: Use this line below in newer versions of Drupal instead of jumping through the hoops below. See: https://api.drupal.org/comment/49318#comment-49318
      //field_attach_form('user', $user, $form, $form_state, NULL, array('field_name' => 'field_mobile_number'));

      // Since the field_attach_form does not yet have the option to get one field, get all in a temp form.
      $tmpform = array();
      $tmpform_state = array();

      // Create the temporary form/state by reference.
      field_attach_form('user', $account, $tmpform, $tmpform_state);
      // Place a copy of the new form field within the actual form
      $form['step1']['field_mobile_number'] = $tmpform['field_mobile_number'];

      if (isset($form['step1']['field_mobile_number']['#weight'])) {
        unset($form['step1']['field_mobile_number']['#weight']);
      }

      // Copy over the $form_state field element as well to avoid notices <code>Undefined index .. field in field_widget_field() line 578</code>
      $form_state['step1']['field']['field_mobile_number'] = $tmpform_state['field']['field_mobile_number'];

      // If the user has put the phone number in the wizard already, use it instead of the phone number in the profile.
      if (isset($form_state['storage']['values']['step1']['field_mobile_number'])) {
        // Get number from wizard form storage.
        $form['step1']['field_mobile_number'][LANGUAGE_NONE][0]['#default_value']['number'] = $form_state['storage']['values']['step1']['field_mobile_number'][LANGUAGE_NONE][0]['number'];
        $form['step1']['field_mobile_number'][LANGUAGE_NONE][0]['#default_value']['country_codes'] = $form_state['storage']['values']['step1']['field_mobile_number'][LANGUAGE_NONE][0]['country_codes'];
      }
      break;

    case 2:

      $form['step2'] = array(
        '#type' => 'fieldset',
        '#title' => t('Step 2 of 2: Enter verification code'),
      );

      // The validate token form field.
      $form['step2']['sms_token'] = array(
        '#type' => 'textfield',
        '#title' => t('Please enter SMS verification code'),
        '#description' => 'The verification number is case sensitive.',
        '#default_value' => '',
        '#size' => 60,
        '#maxlength' => 128,
        '#required' => TRUE,
      );

      break;
  }

  if ($step > 1) {
    $form['prev'] = array(
      '#type' => 'submit',
      '#value' => '&laquo; ' . t("Previous"),
      // Since all info will be discarded, don't validate on 'prev'.
      '#limit_validation_errors' => array(),
      // #submit is required to use #limit_validation_errors
      '#submit' => array('swisscom_smstoken_verify_form_submit'),
      '#ajax' => array(
        'wrapper' => 'wizard-form-wrapper',
        'callback' => 'swisscom_smstoken_wizard_callback',
      ),
    );
  }


  if ($step == 1) {
    $form['next'] = array(
      '#type' => 'submit',
      '#value' => t('Next') . ' &raquo;',
      '#ajax' => array(
        'wrapper' => 'wizard-form-wrapper',
        'callback' => 'swisscom_smstoken_wizard_callback',
      ),
    );
  }
  else {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Validate') . ' &raquo;',
    );
  }

  // Allow persons w/proper permissions to skip mobile phone verification.
  if (user_access('bypass mobile phone validation')) {
    drupal_set_message('Notice: You have permissions to skip this validation process.', 'status');
    $form['skip'] = array(
      '#type' => 'submit',
      '#value' => t('Skip verification') . ' &raquo;',
    );
  }

  return $form;
}

function swisscom_smstoken_wizard_callback($form, $form_state) {
  return $form;
}


/**
 * Form constructor for the SMS verify form.
 */
function swisscom_smstoken_verify_form_validate($form, &$form_state) {
  // Ignore previous button
  if($form_state['triggering_element']['#value'] == '&laquo; ' . t('Previous')) {
    return;
  }

  // Ignore skip button
  if($form_state['triggering_element']['#value'] == t('Skip verification') . ' &raquo;') {
    return;
  }


  // Get the current step
  $current_step = 'step' . $form_state['storage']['step'];

  // If they entered a mobile number, send token verification.
  if ($current_step == 'step1') {
    $mobile_country_code = $form_state['values']['step1']['field_mobile_number'][LANGUAGE_NONE][0]['country_codes'];
    $mobile_number = $form_state['values']['step1']['field_mobile_number'][LANGUAGE_NONE][0]['number'];
    if (isset($mobile_number)) {
      swisscom_smstoken_send_sms_validation_code($mobile_country_code, $mobile_number);
    }
  }

  // Verify the verification code.
  if ($current_step == 'step2') {
    $sms_token = $form_state['values']['step2']['sms_token'];
    $mobile_country_code = $form_state['storage']['values']['step1']['field_mobile_number'][LANGUAGE_NONE][0]['country_codes'];
    $mobile_number = $form_state['storage']['values']['step1']['field_mobile_number'][LANGUAGE_NONE][0]['number'];
    if (isset($sms_token)) {
      $status = swisscom_smstoken_validate_sms_validation_code($mobile_country_code, $mobile_number, $sms_token);
      if ($status) {
        $form_state['storage']['is_validated'] = TRUE;
      }
    }
  }

}
/**
 * Helper handler to redirect user to home page after verifying SMS pin
 *
 * @param array $form
 * @param array $form_state
 */
function swisscom_smstoken_verify_form_submit($form, &$form_state) {
  global $user;

  if ($form_state['triggering_element']['#value'] == t('Skip verification') . ' &raquo;') {
    // Set "sms token validated" to Yes.
    $obj = entity_metadata_wrapper('user', $user);
    $obj->field_sms_token_verified = 1;
    $obj->save();
    $form_state['rebuild'] = FALSE;
    $form_state['redirect'] = 'user/me/apps';
    return;
  }

  // Save away the current information.
  $current_step = 'step' . $form_state['storage']['step'];
  if (!empty($form_state['values'][$current_step])) {
    $form_state['storage']['values'][$current_step] = $form_state['values'][$current_step];
  }

  // Increment or decrement the step as needed. Recover values if they exist.
  if ($form_state['triggering_element']['#value'] == t('Next') . ' &raquo;') {
    $form_state['storage']['step']++;
    // If values have already been entered for this step, recover them from
    // $form_state['storage'] to pre-populate them.
    $step_name = 'step' . $form_state['storage']['step'];
    if (!empty($form_state['storage']['values'][$step_name])) {
      $form_state['values'][$step_name] = $form_state['storage']['values'][$step_name];
    }
  }
  if ($form_state['triggering_element']['#value'] == '&laquo; ' . t('Previous')) {
    $form_state['storage']['step']--;
    // Recover our values from $form_state['storage'] to pre-populate them.
    $step_name = 'step' . $form_state['storage']['step'];
    $form_state['values'][$step_name] = $form_state['storage']['values'][$step_name];
    // We are going back a step, so do not do the submit logic below.
    $form_state['rebuild'] = TRUE;
    return;
  }

//  // If they entered a mobile number, send token verification
//  if ($current_step == 'step1') {
//    $mobile_country_code = $form_state['values']['step1']['field_mobile_number'][LANGUAGE_NONE][0]['country_codes'];
//    $mobile_number = $form_state['values']['step1']['field_mobile_number'][LANGUAGE_NONE][0]['number'];
//    if (isset($mobile_number)) {
//      swisscom_smstoken_send_sms_validation_code($mobile_country_code, $mobile_number);
    
//    }
//  }
//
  // Verify the verification code.
  if ($current_step == 'step2') {
      if (isset($form_state['storage']['is_validated'])) {
        $form_state['rebuild'] = FALSE;
        $form_state['redirect'] = 'user/me/apps';
        return;
      }
  }

  $form_state['rebuild'] = TRUE;

}

/**
 * Select the 'box' element, change the markup in it, and return it as a
 * renderable array.
 *
 * @param $form
 * @param $form_state
 * @return renderable array (the box element)
 */
function swisscom_smstoken_sendsms_ajax_callback($form, $form_state) {
  // In most cases, it is recommended that you put this logic in form generation
  // rather than the callback. Submit driven forms are an exception, because
  // you may not want to return the form at all.
  $element = $form['box'];
  $element['#markup'] = "Clicked submit ({$form_state['values']['op']}): " . date('c');
  return $element;
}

/**
 * Business logic that will send out mobile number.  This function will properly load
 * and call the REST SMS validation method.
 *
 * @param $country_code The country code from the cck_phone number field
 * @param $mobile_number  The phone number.  Since Swisscom mobile numbers start with
 * a zero, this function will strip the zero before sending.
 */
function swisscom_smstoken_send_sms_validation_code($country_code, $mobile_number) {

  // Trim the first zero from the number if it is a zero
  if (substr($mobile_number, 0, 1) == '0') {
    $mobile_number = substr($mobile_number, 1);
  }

  $phone_country = cck_phone_countrycodes($country_code);
  $phone_full = $phone_country['code'] . $mobile_number;

  module_load_include('php', 'swisscom_smstoken', 'SwisscomSmsToken');
  $client = swisscom_smstoken_smstoken_client();
  $dc = new SwisscomSmsToken($client);
  try {
    $status = $dc->smsToken($phone_full);
    if (!$status) {
      throw new Exception(t("Unable to generate PIN for %mobile_number."));
    }

    drupal_set_message(t("A new SMS pin has been sent to your mobile number " . check_plain($phone_full)));
    return TRUE;
  } catch (Exception $e) {
    watchdog('swiscom_smstoken', 'Error sending SMS Verification PIN: %error', array(
      '%error' => $e->getMessage(),
      WATCHDOG_ERROR
    ));
    form_set_error('',t('Unable to generate a new SMS pin at the moment. Please try again later.'));
    return FALSE;
  }

}

function swisscom_smstoken_validate_sms_validation_code($country_code, $mobile_number, $sms_token) {
  global $user;

  module_load_include('php', 'swisscom_smstoken', 'SwisscomSmsToken');
  $client = swisscom_smstoken_smstoken_client();
  $dc = new SwisscomSmsToken($client);

  // Trim the first zero from the number if it is a zero
  if (substr($mobile_number, 0, 1) == '0') {
    $mobile_number = substr($mobile_number, 1);
  }

  $phone_country = cck_phone_countrycodes($country_code);
  $phone_full = $phone_country['code'] . $mobile_number;

  try {
    $is_validated = $dc->validateSmsToken($phone_full, check_plain($sms_token));
  } catch (Exception $e) {
    watchdog('swiscom_smstoken', 'Error validating SMS PIN: %error', array(
      '%error' => $e->getMessage(),
      WATCHDOG_ERROR
    ));
    form_set_error('step2][sms_token',t('Verification pin <b>' . check_plain($sms_token) . '</b> could not be validated at this time. Please try again later.'));
    return FALSE;
  }

  if (!$is_validated) {
    form_set_error('step2][sms_token',t('Verification pin entered <b>' . check_plain($sms_token) . '</b> is invalid or expired. Please re-enter.'));
    return FALSE;
  }
  else {

    // load the user object
    $account = user_load($user->uid);

    // Update the user's phone number and county code w/validated mobile number.
    $update_values = array();
    $update_values['field_mobile_number'][LANGUAGE_NONE][0]['number'] = check_plain($mobile_number);
    $update_values['field_mobile_number'][LANGUAGE_NONE][0]['country_codes'] = $country_code;
    $update_values['field_mobile_number'][LANGUAGE_NONE][0]['extension'] = NULL;
    $update_values['field_sms_token_verified'][LANGUAGE_NONE][0]['value'] = 1;
    $account = user_save($account, $update_values);
    swisscom_smstoken_sync_mobile($account, $phone_full);
    drupal_set_message(t("Congratulations, your mobile number %number has been verified.", array('%number' => check_plain($phone_full))));
    return TRUE;
  }

}
