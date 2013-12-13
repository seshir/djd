<?php
/**
 * @file
 * Administration Settings for Swisscom Custom module.
 */

/**
 * Form builder; Configure site settings.
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function swisscom_custom_admin_settings() {
  $form = array();

  $form['description']['#markup'] = t('This page contains custom settings for Swisscom.');

  $form['general'] = array(
    '#type' => 'fieldset',
    '#title' => t('General'),
  );

  $form['general']['swisscom_custom_403_error_is_removed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Do not respond with HTTP 403 status codes'),
    '#description' => t('The Swisscom server on the development environment ' .
      'intercepts 403 errors and shows it\'s own 403 error page. ' .
      'Select this option to send back HTTP 200 even on access denied pages.'),
    '#default_value' => variable_get('swisscom_custom_403_error_is_removed', FALSE),
  );

  return system_settings_form($form);
}
