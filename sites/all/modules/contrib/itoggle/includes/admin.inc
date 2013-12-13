<?php
/**
 * @file
 * iToggle settings.
 */

/**
 * Form callback.
 * Return iToggle settings form.
 */
function itoggle_form_admin() {
  $form = array();

  $form['itoggle']['basic'] = array(
    '#type' => 'fieldset',
    '#title' => t('Basic Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['itoggle']['basic']['itoggle_compression_type'] = array(
    '#type' => 'radios',
    '#title' => t('Choose iToggle compression level'),
    '#options' => array(
      'min' => t('Production (Minified)'),
      'none' => t('Development (Uncompressed Code)'),
    ),
    '#default_value' => variable_get('itoggle_compression_type', 'min'),
  );

  $form['itoggle']['basic']['itoggle_css'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include Default CSS'),
    '#description' => t('Uncheck this if you want to include your own styles for iToggle.'),
    '#default_value' => variable_get('itoggle_css', TRUE),
  );

  $form['itoggle']['basic']['itoggle_speed'] = array(
    '#type' => 'textfield',
    '#size' => 4  ,
    '#title' => t('Animation Speed'),
    '#description' => t('iToggle animation speed, in milliseconds. Default is 200.'),
    '#default_value' => variable_get('itoggle_speed', 200),
  );

  $form['itoggle']['basic']['easing'] = array(
    '#type' => 'fieldset',
    '#title' => t('Easing'),
    '#description' => t('If you installed the <a href="@link">jQuery Easing</a> plugin you can specify an easing function for iToggle here. <strong>Easing is highly recommended</strong> and makes animations much sleeker.', array('@link' => 'http://gsgd.co.uk/sandbox/jquery/easing/')),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['itoggle']['basic']['easing']['itoggle_easing_use'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable Easing'),
    '#description' => t('Enable easing for sleeker animations.'),
    '#default_value' => variable_get('itoggle_easing_use', FALSE),
  );

  $form['itoggle']['basic']['easing']['itoggle_easing_function'] = array(
    '#type' => 'textfield',
    '#title' => t('Easing Function'),
    '#description' => t("Specify an easing callback here. iToggle default is 'easeOutExpo'."),
    '#default_value' => variable_get('itoggle_easing_function', 'easeOutExpo'),
    '#states' => array(
      'visible' => array(
        'input[name="itoggle_easing_use"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['advanced'] = array(
    '#title' => t('Advanced Settings'),
    '#type' => 'fieldset',
    '#description' => '<p>' . t('For a full description of all iToggle callbacks, see the <a href="@link">documentation</a>.', array('@link' => 'http://labs.engageinteractive.co.uk/itoggle/')) . '</p>',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['advanced']['note'] = array(
    '#markup' => '<strong>' . t('Important') . '</strong><p>' . t("All of these callbacks are automatically wrapped in a closure, so you do don't need to specify it, just enter the code directly.") . '</p>',
  );

  $form['advanced']['example'] = array(
    '#markup' => '<p>' . t('For example') . ':</p><code>alert("onclick");</code></p>',
  );

  $form['advanced']['warning'] = array(
    '#markup' => '<strong>' . t('Warning') . '</strong><p>' . t('The callbacks are called using eval(), yes, we hate it too so please be careful!') . '</p>',
  );

  $form['advanced']['advanced_confirm'] = array(
    '#type' => 'checkbox',
    '#title' => t('I confirm that I have read the above warnings and wish to configure callbacks for iToggle'),
    '#default_value' => FALSE,
  );

  $form['advanced']['container'] = array(
    '#type' => 'container',
    '#states' => array(
      'invisible' => array(
        'input[name="advanced_confirm"]' => array('checked' => FALSE),
      ),
    ),
  );

  $form['advanced']['container']['itoggle_onclick'] = array(
    '#title' => t('OnClick callback'),
    '#type' => 'textarea',
    '#description' => t('Specify an OnClick callback for iToggle.'),
    '#default_value' => variable_get('itoggle_onclick', ''),
  );

  $form['advanced']['container']['itoggle_onclickon'] = array(
    '#title' => t('OnClickOn callback'),
    '#type' => 'textarea',
    '#description' => t('Specify an OnClickOn callback for iToggle.'),
    '#default_value' => variable_get('itoggle_onclickon', ''),
  );

  $form['advanced']['container']['itoggle_onclickoff'] = array(
    '#title' => t('OnClickOff callback'),
    '#type' => 'textarea',
    '#description' => t('Specify an OnClickOff callback for iToggle.'),
    '#default_value' => variable_get('itoggle_onclickoff', ''),
  );

  $form['advanced']['container']['itoggle_onslide'] = array(
    '#title' => t('OnSlide callback'),
    '#type' => 'textarea',
    '#description' => t('Specify an OnSlide callback for iToggle.'),
    '#default_value' => variable_get('itoggle_onslide', ''),
  );

  $form['advanced']['container']['itoggle_onslideon'] = array(
    '#title' => t('OnSlideOn callback'),
    '#type' => 'textarea',
    '#description' => t('Specify an OnSlideOn callback for iToggle.'),
    '#default_value' => variable_get('itoggle_onslideon', ''),
  );

  $form['advanced']['container']['itoggle_onslideoff'] = array(
    '#title' => t('OnSlideOff callback'),
    '#type' => 'textarea',
    '#description' => t('Specify an OnSlideOff callback for iToggle.'),
    '#default_value' => variable_get('itoggle_onslideoff', ''),
  );

  return system_settings_form($form);
}

/**
 * Validation callback.
 * Checks for jQuery Easing library if option is selected.
 *
 * @see itoggle_form_admin()
 */
function itoggle_form_admin_validate($form, &$form_state) {
  if ($form_state['values']['itoggle_easing_use'] && !itoggle_check_easing_plugin_path()) {
    form_set_error('itoggle_easing_use', t("You need to download the !easing and save the 'jquery.easing.1.3.js' file into the %path folder of your server.", array('!easing' => l(t('jQuery Easing plugin'), 'http://gsgd.co.uk/sandbox/jquery/easing/'), '%path' => ITOGGLE_EASING_PATH)));
  }
}

/**
 * Checks if the directory in $form_element exists and contains a file named
 * 'engage.itoggle.js'. If validation fails, the form element is flagged
 * with an error from within the file_check_directory function.
 *
 * This was borrowed from the Colorbox module.
 *
 * @param $form_element
 *   The form element containing the name of the directory to check.
 *
 * @link http://drupal.org/project/colorbox
 */
function _itoggle_admin_check_plugin_path($form_element) {
  $library_path = $form_element['#value'];

  if (!itoggle_check_plugin_path($library_path)) {
    form_set_error($form_element['#parents'][0], t('You need to download the !itoggle and extract the entire contents of the archive into the %path folder of your server.', array('!itoggle' => l(t('iToggle plugin'), 'http://labs.engageinteractive.co.uk/itoggle/'), '%path' => $library_path)));
  }

  return $form_element;
}