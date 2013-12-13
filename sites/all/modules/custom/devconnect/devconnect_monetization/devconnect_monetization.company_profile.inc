<?php

use Apigee\Commerce\Organization;
use Apigee\Commerce\DataStructures\DeveloperTnc;
use Apigee\Commerce\TermAndCondition;
use Apigee\Commerce\DataStructures\Address;
use Apigee\ManagementAPI\Developer;
use Apigee\Commerce\Types\Country;
use Apigee\Commerce\Types\BillingType;
use Apigee\Commerce\Types\DeveloperType;

function devconnect_monetization_company_profile_form () {
  $client = devconnect_default_api_client();
  $developer_id = _devconnect_monetization_get_developer_id();

  $org = new Organization($client);
  $org->load();

  $org_timezone = new DateTimeZone($org->getTimezone());
  $local_timezone = new DateTimeZone(date_default_timezone_get());

  // Contact Info tab
  $company_profile_form = drupal_get_form('devconnect_monetization_company_details_form');
  $company_bank_form = drupal_get_form('devconnect_monetization_company_bank_details_form');

  // Manage Users tab
  $manage_users_form = drupal_get_form('devconnect_monetization_roles_form');
  $associate_new_user_form = drupal_get_form('devconnect_monetization_users_add_to_company_form');

  $tnc = new TermAndCondition($client);
  $tncs = $tnc->getList(TRUE);

  // Sort TnCs by start date DESC
  uasort($tncs, function($tnc1, $tnc2) {
    $date1 = date_create_from_format('Y-m-d', $tnc1->getStartDate());
    $date2 = date_create_from_format('Y-m-d', $tnc2->getStartDate());

    if ($date1 < $date2) return 1;
    if ($date1 > $date2) return -1;
    return 0;
  });

  $dev_tncs = $tnc->getAcceptedDevTermsAndConditions($developer_id);

  $tncs_forms = array();
  foreach ($tncs as $tnc) {
    $form = drupal_get_form('devconnect_monetization_accept_tnc_form', $tnc);
    $tncs_form = array(
      'tnc' => $tnc,
      'form' => drupal_render($form),
    );

    foreach ($dev_tncs as $dev_tnc) {
      if ($dev_tnc->getTnc()->getId() == $tnc->getId()) {
        $tncs_form['accepted'] = DateTime::createFromFormat('Y-m-d H:i:s', $dev_tnc->getAuditDate(), $org_timezone)->setTimezone($local_timezone)->format('Y-m-d H:i:s');
        break;
      }
    }
    $tncs_forms[] = $tncs_form;
  }

  /*********************************************************
   * If developers have not accepted TnC, they should only *
   * see the current and future TnC. Not old expired ones  *
   * (unless they had accepted them)                       *
   * Should only see the CURRENT (and next future) one.    *
   *********************************************************/

  $remove = FALSE;
  for ($i = 0, $len = count($tncs_forms); $i < $len; $i++) {
    if (!$remove && isset($tncs_forms[$i]['accepted'])) {
      $remove = TRUE;
      continue;
    }
    if ($remove && !isset($tncs_forms[$i]['accepted'])) {
      unset($tncs_forms[$i]);
    }
  }

  $variables = array(
    'company_profile_form'    => drupal_render($company_profile_form),
    'company_bank_form'       => drupal_render($company_bank_form),
    'manage_users_form'       => drupal_render($manage_users_form),
    'associate_new_user_form' => drupal_render($associate_new_user_form),
    'tncs_collection'         => $tncs_forms,
  );

  $output = theme('devconnect_monetization_company_profile', $variables);
  drupal_set_title(t('Company Profile'));
  return $output;
}

function devconnect_monetization_company_details_form () {
  $developer_id = _devconnect_monetization_get_developer_id();
  $client = devconnect_default_api_client();
  $developer = new \Apigee\Commerce\Developer($client);
  $developer->load($developer_id);

  $dev = new Developer($client);
  $dev->load($developer_id);

  $form['#attributes']['class'][] = 'form-horizontal';

  $form['company_details'] = array(
    '#type' => 'fieldset',
    '#title' => t('Company Details'),
    '#collapsible' => FALSE,
  );

  $form['company_details']['legal_company_name'] = array(
    '#title' => t('Legal Company Name'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value' => $dev->getAttribute('COMMERCE_DEVELOPER_LEGAL_NAME'),
  );

  $form['company_details']['company_reg_number'] = array(
    '#title' => t('Company Reg. Number'),
    '#type' => 'textfield',
    '#default_value' => $dev->getAttribute('COMMERCE_REGISTRATION_ID'),
  );

  foreach($developer->getAddresses() as $address_item) {
    if ($address_item->isPrimary()) {
      $address = $address_item;
      break;
    }
  }

  if (!isset($address)) {
    $address = new Address(array('country' => 'US', 'isPrimary' => TRUE));
  }

  $form['billing_address'] = array(
    '#type' => 'fieldset',
    '#title' => t('Billing Address'),
    '#collapsible' => FALSE,
  );

  $form['address_id'] = array(
    '#type' => 'value',
    '#value' => $address->getId(),
  );
  $form['billing_address']['street_address_1'] = array(
    '#title' => t('Street Address 1'),
    '#type' => 'textfield',
    '#default_value' => $address->getAddress1(),
    '#required' => TRUE,
  );

  $form['billing_address']['street_address_2'] = array(
    '#title' => t('Street Address 2'),
    '#type' => 'textfield',
    '#default_value' => $address->getAddress2(),
  );

  $form['billing_address']['country'] = array(
    '#title' => t('Country'),
    '#type' => 'select',
    '#options' => Country::getList(),
    '#default_value' => $address->getCountry(),
    '#empty_option' => t('Select Country'),
  );

  $form['billing_address']['state_province'] = array(
    '#title' => t('State\Province'),
    '#type' => 'textfield',
    '#default_value' => $address->getState(),
  );

  $form['billing_address']['city'] = array(
    '#title' => t('City'),
    '#type' => 'textfield',
    '#default_value' => $address->getCity(),
    '#required' => TRUE,
  );

  $form['billing_address']['zip_code'] = array(
    '#title' => t('Zip/Postal Code'),
    '#type' => 'textfield',
    '#default_value' => $address->getZip(),
    '#required' => TRUE,
  );

  $form['_billing_type_fieldset'] = array(
    '#title' => t('Billing Type'),
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
  );

  $form['_billing_type_fieldset']['_billing_type'] = array(
    '#markup' => ucfirst(strtolower($dev->getAttribute('COMMERCE_BILLING_TYPE'))),
  );

  $form['contact_info'] = array(
    '#type' => 'fieldset',
    '#title' => t('Contact Info'),
    '#collapsible' => FALSE,
  );

  $form['contact_info']['contact_name'] = array(
    '#type' => 'item',
    '#title' => t('Contact Name'),
    '#markup' => t('@first_name @last_name', array('@first_name' => $dev->getFirstName(), '@last_name' => $dev->getLastName())),
  );

  $form['contact_info']['contact_tel_number'] = array(
    '#title' => t('Telephone Number'),
    '#type' => 'textfield',
    '#default_value' => $dev->getAttribute('COMMERCE_DEVELOPER_PHONE'),
  );

  $form['contact_info']['contact_email'] = array(
    '#type' => 'item',
    '#title' => t('Email Address'),
    '#markup' => t('@email', array('@email' => $developer->getEmail())),
  );

  $form['vat_and_tax_detail'] = array(
    '#title' => t('VAT &amp; Tax Details'),
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
  );

  $form['vat_and_tax_detail']['registered_for_tax_vat'] = array(
    '#title' => t('Are you registered for Tax/VAT?'),
    '#type' => 'radios',
    '#options' => array(1 => t('Yes'), 0 => t('No')),
    '#default_value' => strlen(trim($dev->getAttribute('COMMERCE_TAX_EXEMPT_AUTH_NO'))) > 0 ? 1 : 0,
  );

  $form['vat_and_tax_detail']['vat_tax_number'] = array(
    '#type' => 'textfield',
    '#title' => t('VAT/Tax Number'),
    '#default_value' => trim($dev->getAttribute('COMMERCE_TAX_EXEMPT_AUTH_NO')),
  );

  $form['developer_type'] = array(
    '#type' => 'value',
    '#value' => $dev->getAttribute('COMMERCE_DEVELOPER_TYPE'),
  );

  $form['billing_type'] = array(
    '#type' => 'value',
    '#value' => $dev->getAttribute('COMMERCE_BILLING_TYPE'),
  );

  $form['company_id'] = array(
    '#type' => 'value',
    '#value' => $dev->getAttribute('COMMERCE_COMPANY_ID'),
  );

  $form['approx_tax_rate'] = array(
    '#type' => 'value',
    '#value' => $dev->getAttribute('COMMERCE_APPROX_TAX_RATE'),
  );

  // Note that KMS does not return boolean values but it string representation
  // That will break custom fields if they are checkboxes
  $form['is_broker'] = array(
    '#type' => 'value',
    '#value' => $dev->getAttribute('COMMERCE_IS_BROKER') == 'true' ? 1 : 0,
  );

  $form['has_self_billing'] = array(
    '#type' => 'value',
    '#value' => $dev->getAttribute('COMMERCE_HAS_SELF_BILLING'),
  );

  $form['billing_profile'] = array(
    '#type' => 'value',
    '#value' => $dev->getAttribute('COMMERCE_BILLING_PROFILE'),
  );

  $form['supported_currency'] = array(
    '#type' => 'value',
    '#value' => $dev->getAttribute('COMMERCE_SUPPORTED_CURRENCY'),
  );

  $form['developer_category'] = array(
    '#type' => 'value',
    '#value' => $dev->getAttribute('COMMERCE_DEVELOPER_CATEGORY'),
  );

  $form['developer_roles'] = array(
    '#type' => 'value',
    '#value' => $dev->getAttribute('COMMERCE_ROLES'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Changes'),
  );

  $form['cancel'] = array(
      '#markup' => l('Cancel', 'users/me/monetization/company/edit'),
  );

  $form['#submit'][] = 'devconnect_monetization_company_details_form_validate';
  $form['#submit'][] = 'devconnect_monetization_company_details_form_submit';
  return $form;
}

function devconnect_monetization_company_details_form_validate(&$form, &$form_state) {
  if ($form_state['values']['registered_for_tax_vat'] == 1 && strlen(trim($form_state['values']['vat_tax_number'])) == 0) {
    form_set_error('vat_tax_number', t('If you are registered for Tax/VAT, then you must enter you VAT/Tax number'));
  }
}

function devconnect_monetization_company_details_form_submit(&$form, &$form_state) {
  try {
    $developer_id = _devconnect_monetization_get_developer_id();
    if ($GLOBALS['user']->mail != $developer_id) {
      $uid = user_load_by_mail($developer_id)->uid;
    }
    else {
      $uid = $GLOBALS['user']->uid;
    }

    $account = entity_load_single('user', $uid);

    $account->field_comm_developer_legal_name[LANGUAGE_NONE][0]['value'] = $form_state['values']['legal_company_name'];
    $account->field_comm_registration_id[LANGUAGE_NONE][0]['value'] = $form_state['values']['company_reg_number'];
    if ($form_state['values']['registered_for_tax_vat'] == 1) {
      $account->field_comm_vat_tax_number[LANGUAGE_NONE][0]['value'] = $form_state['values']['vat_tax_number'];
    }
    $account->field_comm_developer_phone[LANGUAGE_NONE][0]['value'] = $form_state['values']['contact_tel_number'];
    $address = array(
        'address1' => $form_state['values']['street_address_1'],
        'address2' => $form_state['values']['street_address_2'],
        'city' => $form_state['values']['city'],
        'country' => $form_state['values']['country'],
        'isPrimary' => 'true',
        'state' => $form_state['values']['state_province'],
        'zip' => $form_state['values']['zip_code'],
    );
    if (isset($form_state['values']['address_id']) && !is_null($form_state['values']['address_id'])) {
      $address['id'] = $form_state['values']['address_id'];
    }
    $account->field_comm_developer_address[LANGUAGE_NONE][0]['value'] = json_encode($address, JSON_FORCE_OBJECT);
    $account->field_comm_developer_type[LANGUAGE_NONE][0]['value'] = $form_state['values']['developer_type'];
    $account->field_comm_billing_type[LANGUAGE_NONE][0]['value'] = $form_state['values']['billing_type'];
    $account->field_comm_company_id[LANGUAGE_NONE][0]['value'] = $form_state['values']['company_id'];
    $account->field_comm_approx_tax_rate[LANGUAGE_NONE][0]['value'] = $form_state['values']['approx_tax_rate'];
    $account->field_comm_is_broker[LANGUAGE_NONE][0]['value'] = $form_state['values']['is_broker'];
    $account->field_comm_has_self_billing[LANGUAGE_NONE][0]['value'] = $form_state['values']['has_self_billing'];
    $account->field_comm_billing_profile[LANGUAGE_NONE][0]['value'] = $form_state['values']['billing_profile'];
    $account->field_comm_supported_currency[LANGUAGE_NONE][0]['value'] = $form_state['values']['supported_currency'];
    $account->field_comm_developer_category[LANGUAGE_NONE][0]['value'] = $form_state['values']['developer_category'];
    $account->field_comm_developer_roles[LANGUAGE_NONE][0]['value'] = $form_state['values']['developer_roles'];

    entity_save('user', $account);
    drupal_set_message('Your Company Profile Info has been saved.', 'status');
  }
  catch(\Exception $e) {
    drupal_set_message('The website encountered an unexpected error. Please try again later. ', 'error');
    watchdog_exception('devconnect_monetization', $e, $e->getMessage(), array(), WATCHDOG_CRITICAL);
  }
}

function devconnect_monetization_company_bank_details_form() {

  $form['#attributes']['class'][] = 'form-horizontal';

  $form['bank_details'] = array(
    '#type' => 'fieldset',
    '#title' => t('Bank Details'),
    '#collapsible' => FALSE,
  );

  $form['bank_details']['street_address_1'] = array(
      '#title' => t('Street Address 1'),
      '#type' => 'textfield',
      '#default_value' => $address['address1'],
  );

  $form['bank_details']['street_address_2'] = array(
      '#title' => t('Street Address 2'),
      '#type' => 'textfield',
      '#default_value' => $address['address2'],
  );

  $form['bank_details']['country'] = array(
      '#title' => t('Country'),
      '#type' => 'select',
      '#options' => Country::getList(),
      "#empty_option"=>t('Select Country'),
  );

  $form['bank_details']['state_province'] = array(
      '#title' => t('State\Province'),
      '#type' => 'textfield',
      '#default_value' => $address['state'],
  );

  $form['bank_details']['zip_code'] = array(
      '#title' => t('Zip/Postal Code'),
      '#type' => 'textfield',
      '#default_value' => $address['zip'],
  );

  $form['account_info'] = array(
    '#title' => t('Account Info'),
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
  );

  $form['account_info']['account_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Account Name'),
    '#required' => TRUE,
  );

  $form['account_info']['account_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Account Number'),
  );

  $form['account_info']['account_currency'] = array(
    '#type' => 'select',
    '#title' => t('Currency'),
    '#options' => array('CHF', 'USD'),
  );

  $form['account_info']['account_sort_code'] = array(
    '#type' => 'textfield',
    '#title' => t('Sort Code'),
  );

  $form['account_info']['account_aba_routing_number'] = array(
    '#type' => 'textfield',
    '#title' => t('ABA/Routing Number'),
  );

  $form['account_info']['account_bic_swiff_code'] = array(
    '#type' => 'textfield',
    '#title' => t('BIC/Swiff Code')
  );

  $form['account_info']['account_iban'] = array(
    '#type' => 'textfield',
    '#title' => t('IBAN')
  );

  $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save Changes'),
  );

  $form['cancel'] = array(
      '#markup' => l('Cancel', 'users/me/monetization/company/edit'),
  );

  return $form;
 }

/* Monetization Roles */
function devconnect_monetization_roles_form() {

  $client = devconnect_monetization_default_api_client();
  $developer_id = _devconnect_monetization_get_developer_id();

  $query = new EntityFieldQuery();
  $child_developers = $query
    ->entityCondition('entity_type', 'user')
    ->entityCondition('bundle', 'user')
    ->fieldCondition('field_comm_company_id', 'value', $developer_id, '=')
    ->execute();

  $child_ids = array_keys($child_developers['user']);

  $developers = count($child_ids) > 0 ? entity_load('user', $child_ids) : array();
  $developers = entity_load('user', array($GLOBALS['user']->uid)) + $developers;
  $form = array();
  $dev_list = array();
  foreach ($developers as $uid => $developer) {
    $dev_list[$uid] = array(
      'name' => array(
        '#type' => 'item',
        '#markup' => $developer->field_first_name[LANGUAGE_NONE][0]['value'] . ' ' . $developer->field_last_name[LANGUAGE_NONE][0]['value']
      ),
      "admin[$uid]" => array(
        '#type' => 'checkbox',
        '#default_value' => in_array(MONETIZATION_ADMIN_ROLE_NAME, $developer->roles),
      ),
      "finance[$uid]" => array(
        '#type' => 'checkbox',
        '#default_value' => in_array(MONETIZATION_FINANCE_ADMIN_ROLE_NAME, $developer->roles),
      ),
      "developer[$uid]" => array(
        '#type' => 'checkbox',
        '#default_value' => in_array(MONETIZATION_DEVELOPER_ROLE_NAME, $developer->roles),
      ),
      "action[$uid]" => array(
        '#markup' => l(t('Delete'), 'users/me/monetization/company/remove-developer/' . $uid, array('attributes' => array('class' => array('btn'))))
      )
    );
  }

  // Keep loggin user from removing his roles
  $dev_list[$GLOBALS['user']->uid]["admin[{$GLOBALS['user']->uid}]"]['#disabled'] = TRUE;
  $dev_list[$GLOBALS['user']->uid]["finance[{$GLOBALS['user']->uid}]"]['#disabled'] = TRUE;
  $dev_list[$GLOBALS['user']->uid]["developer[{$GLOBALS['user']->uid}]"]['#disabled'] = TRUE;
  $dev_list[$GLOBALS['user']->uid]["action[{$GLOBALS['user']->uid}]"]['#access'] = FALSE;

  $form['developers'] = $dev_list;

  $form['submit'][] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );
  $form['#theme'] = 'devconnect_monetization_roles_form';
  $form['#submit'][] = 'devconnect_monetization_roles_form_submit';
  return $form;
}

function devconnect_monetization_roles_form_submit($form, &$form_state) {
  try {

    $developer_id = _devconnect_monetization_get_developer_id();

    $query = new EntityFieldQuery();
    $child_developers = $query
      ->entityCondition('entity_type', 'user')
      ->entityCondition('bundle', 'user')
      ->fieldCondition('field_comm_company_id', 'value', $developer_id, '=')
      ->execute();

    $child_ids = array_keys($child_developers['user']);

    $developers = count($child_ids) > 0 ? entity_load('user', $child_ids) : $child_ids;
    $exclude_roles = array(
      MONETIZATION_ADMIN_ROLE_NAME => user_role_load_by_name(MONETIZATION_ADMIN_ROLE_NAME),
      MONETIZATION_FINANCE_ADMIN_ROLE_NAME => user_role_load_by_name(MONETIZATION_FINANCE_ADMIN_ROLE_NAME),
      MONETIZATION_DEVELOPER_ROLE_NAME => user_role_load_by_name(MONETIZATION_DEVELOPER_ROLE_NAME),
    );

    $users = array();
    foreach ($developers as $uid => $developer) {
      if ($developer->field_comm_company_id[LANGUAGE_NONE][0]['value'] != $developer_id) {
        continue;
      }
      foreach (array_values($exclude_roles) as $role) {
        unset($developer->roles[$role->rid]);
      }
      $developer->field_comm_developer_roles[LANGUAGE_NONE] = array();
      $users[$developer->uid] = $developer;
    }

    foreach ($form_state['input'] as $role_name => $role_desc) {
      foreach ($role_desc as $uid => $void) {
        switch($role_name) {
          case 'admin':
            $users[$uid]->roles[$exclude_roles[MONETIZATION_ADMIN_ROLE_NAME]->rid] = MONETIZATION_ADMIN_ROLE_NAME;
            $users[$uid]->field_comm_developer_roles[LANGUAGE_NONE][0]['value'][] = MONETIZATION_ADMIN_ROLE_NAME;
            break;
          case 'finance':
            $users[$uid]->roles[$exclude_roles[MONETIZATION_FINANCE_ADMIN_ROLE_NAME]->rid] = MONETIZATION_FINANCE_ADMIN_ROLE_NAME;
            $users[$uid]->field_comm_developer_roles[LANGUAGE_NONE][0]['value'][] = MONETIZATION_FINANCE_ADMIN_ROLE_NAME;
            break;
          case 'developer':
            $users[$uid]->roles[$exclude_roles[MONETIZATION_DEVELOPER_ROLE_NAME]->rid] = MONETIZATION_DEVELOPER_ROLE_NAME;
            $users[$uid]->field_comm_developer_roles[LANGUAGE_NONE][0]['value'][] = MONETIZATION_DEVELOPER_ROLE_NAME;
            break;
        }
      }
    }
    foreach($users as $developer) {
      $developer->field_comm_developer_roles[LANGUAGE_NONE][0]['value'] = json_encode($developer->field_comm_developer_roles[LANGUAGE_NONE][0]['value']);
      if (isset($developer->mail)) {
        entity_save('user', $developer);
      }
    }
    drupal_set_message('Operation completed succeessfully.', 'status');
  }
  catch (Exception $e) {
    drupal_set_message('Your operation could not complete. Please contact your administrator if problem persists.', 'error');
  }

  $form_state['redirect'] = array('users/me/monetization/company/edit', array('fragment' => 'tab4'));
}

function theme_devconnect_monetization_roles_form($variables) {
  $form = $variables['form'];
  $header = array(
    t('Username'),
    t('Admin'),
    t('Finance'),
    t('Developer'),
    t('Actions'),
  );

  $rows = array();

  foreach (element_children($form['developers']) as $uid) {
    $row = array();
    foreach (element_children($form['developers'][$uid]) as $element) {
      $row[] = drupal_render($form['developers'][$uid][$element]);
    }
    $rows[] = array('data' => $row);
  }

  $output = theme_table(array(
        'header' => $header,
        'rows' => $rows,
        'attributes' => array(),
        'empty' => t('Your company has no developers assigned.'),
      )) . drupal_render($form['submit']) . drupal_render_children($form);
  return $output;
}

function devconnect_monetization_users_add_to_company_form() {

  $form['user_email_address'] = array(
   '#type' => 'textfield',
   '#title' => t('Enter User Email Address'),
   '#required' => TRUE,
   //'#autocomplete_path' => 'users/me/monetization/developers',
 );

 $form['submit'] = array(
   '#type' => 'submit',
   '#value' => t('Add User'),
 );

  return $form;
}

function devconnect_monetization_users_add_to_company_form_validate(&$form, &$form_state) {
  $form_state['values']['user_email_address'] = trim($form_state['values']['user_email_address']);
  $admin_id = _devconnect_monetization_get_developer_id();
  if (trim($form_state['values']['user_email_address']) == $admin_id) {
    form_set_error('user_email_address', t('You cannot add your own user to your company.'));
  }

  $child_user = user_load_by_mail($form_state['values']['user_email_address']);
  if ($child_user == NULL) {
    form_set_error('user_email_address', t('User with email @mail does not exist.', array('@mail' => $form_state['values']['user_email_address'])));
  }
  else {
    $child_entity = entity_load_single('user', $child_user->uid);

    if (strlen($child_entity->field_comm_company_id[LANGUAGE_NONE][0]['value']) > 0 ) {
      form_set_error('user_email_address', t('You can not add this developer since it is already assigned.'));
    }
    else {
      $form_state['child_entity'] = $child_entity;
      return;
    }
  }
  drupal_goto('users/me/monetization/company/edit', array('fragment' => 'tab4'));
}

function devconnect_monetization_users_add_to_company_form_submit($form, &$form_state) {
  try {
    $child_entity = $form_state['child_entity'];
    $child_entity->field_comm_company_id[LANGUAGE_NONE][0]['value'] = $GLOBALS['user']->mail;
    entity_save('user', $child_entity);
    drupal_set_message(t('Developer @developer added to your company.', array('@developer' => $form_state['values']['user_email_address'])));
  }
  catch (Exception $e) {
    drupal_set_message(t('There was an error trying to add @developer to your company', array('@developer' => $form_state['values']['user_email_address'])), 'error');
  }
  $form_state['redirect'] = array('users/me/monetization/company/edit', array('fragment' => 'tab4'));
}

function devconnect_monetization_users_autocomple($string) {
  $client = devconnect_monetization_default_api_client();
  $developer = new Developer($client);
  $developers = $developer->listDevelopers();
  $emails = array();
  foreach ($developers as $developer_item) {
    if (stripos($developer_item, $string) !== FALSE) {
      $emails[$developer_item] = $developer_item;
    }
  }
  return drupal_json_output($emails);
}

function devconnect_monetization_user_remove_from_company($uid) {
  try {
    $child_entity = entity_load_single('user', $uid);
    $child_entity->field_comm_company_id = NULL;
    entity_save('user', $child_entity);
    drupal_set_message(t('Developer @developer removed from your company.', array('@developer' => $form_state['values']['user_email_address'])));
  }
  catch (Exception $e) {
    drupal_set_message($e->getMessage(), 'error');
    drupal_set_message(t('There was an error trying to remove @developer from your company', array('@developer' => $form_state['values']['user_email_address'])), 'error');
  }
  drupal_goto('users/me/monetization/company/edit', array('fragment' => 'tab4'));
}

function devconnect_monetization_accept_tnc_form($form, &$form_state) {
  $tnc = $form_state['build_info']['args'][0];

  $form['tnc_id'] = array(
    '#type' => 'hidden',
    '#value' => $tnc->getId(),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Accept Terms & Conditions'),
  );
  $form['#submit'][] = 'devconnect_monetization_accept_tnc_form_submit';

  return $form;
}

function devconnect_monetization_accept_tnc_form_submit($form, &$form_state) {
  try {
    $client = devconnect_monetization_default_api_client();
    $developer_id = _devconnect_monetization_get_developer_id();
    $tnc_id = $form_state['input']['tnc_id'];

    $org = new Organization($client);
    $org->load();

    $org_timezone = new DateTimeZone($org->getTimezone());

    $dev_tnc = new DeveloperTnc();

    $audit_date = new DateTime('now');
    $dev_tnc->setAuditDate($audit_date->setTimezone($org_timezone)->format('Y-m-d H:i:s'));
    $dev_tnc->setAction('ACCEPTED');
    $tnc = new TermAndCondition($client);
    $tnc->acceptTermsAndConditions($dev_tnc, $developer_id, $tnc_id);
    drupal_set_message('You have accepted terms and conditions successfully.');
  }
  catch (Exception $e) {
    drupal_set_message('Error while trying to accept TnCs. Please try again later.');
    watchdog('monetization', 'Could not accept terms and conditions: ' . $e->getMessage(), null, 'error');
  }
  $form_state['redirect'] = array('users/me/monetization/company/edit', array('fragment' => 'tab3'));
}