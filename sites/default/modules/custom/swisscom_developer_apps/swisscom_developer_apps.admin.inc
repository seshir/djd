<?php
/**
 * @file
 * Administration Settings for Swisscom Developer apps.
 */

/**
 * Administration form.
 */
function swisscom_developer_apps_admin_settings() {
  $form = array();

  $form['description']['#markup'] = t('This page contains custom settings for Swisscom developer API keys.');

  $form['general'] = array(
    '#type' => 'fieldset',
    '#title' => t('General'),
  );

   // Get taxonomy name list to have user select API Name
  $taxonomy_names = taxonomy_vocabulary_get_names();
  $options_api_name_vocab =  array('0' => '-- Select a Value --');
  foreach ($taxonomy_names as $taxonomy_name) {
    $options_api_name_vocab[$taxonomy_name->vid] = $taxonomy_name->name;
  }

  $form['general']['swisscom_developer_apps_vocabulary_id'] = array(
    '#type' => 'select',
    '#title' => t('API Name Vocabulary'),
    '#description' => 'The taxonomy used for API names, which ties to API products.',
    '#options' => (empty($options_api_name_vocab)) ? array() : $options_api_name_vocab,
    '#default_value' => variable_get('swisscom_developer_apps_vocabulary_id',0),
    '#required' => TRUE,
  );

  $form['general']['swisscom_developer_apps_product_field'] = array(
    '#type' => 'textfield',
    '#title' => t('API Product field'),
    '#description' => 'The field on the API Name Vocabulary used to link to the API product in the Apigee management server.',
    '#default_value' => variable_get('swisscom_developer_apps_product_field',''),
    '#required' => TRUE,
  );

  return system_settings_form($form);
}

