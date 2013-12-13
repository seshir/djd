<?php
/**
 * @file
 * iToggle module include.
 */

/**
 * Include iToggle css and javascript files.
 *
 * @param boolean
 *   Whether we are just including iToggle scripts or also our own
 *   settings and script.
 */
function itoggle_include_itoggle($full = TRUE) {
  // Add iToggle default CSS.
  if (variable_get('itoggle_css', TRUE)) {
    drupal_add_css(drupal_get_path('module', 'itoggle') . '/misc/itoggle.css');
  }

  // Add jQuery Easing plugin
  if (variable_get('itoggle_easing_use', FALSE)) {
    drupal_add_js(itoggle_get_easing_js(), array('group' => JS_LIBRARY, 'weight' => 0));
  }

  // Add iToggle library.
  drupal_add_js(itoggle_get_js(), array('group' => JS_LIBRARY, 'weight' => 1));

  // Add iToggle default JavaScript and settings.
  if ($full === TRUE) {
    itoggle_include_settings();
    drupal_add_js(drupal_get_path('module', 'itoggle') . '/misc/itoggle.js', array('group' => JS_THEME));
  }
}

/**
 * Add iToggle settings to Drupal.settings object.
 *
 * @see itoggle_include_itoggle()
 */
function itoggle_include_settings() {
  $settings = array(
    'itoggle' => array(
      'speed' => variable_get('itoggle_speed', 200),
      'onclick' => variable_get('itoggle_onclick', ''),
      'onclickon' => variable_get('itoggle_onclickon', ''),
      'onclickoff' => variable_get('itoggle_onclickoff', ''),
      'onslide' => variable_get('itoggle_onslide', ''),
      'onslideon' => variable_get('itoggle_onslideon', ''),
      'onslideoff' => variable_get('itoggle_onslideoff', ''),
    )
  );

  if (variable_get('itoggle_easing_use', FALSE)) {
    $settings['itoggle']['easing'] = variable_get('itoggle_easing_function', 'easeOutExpo');
  }

  drupal_add_js($settings, 'setting');
}

/**
 * Return the JS filename for iToggle plugin.
 * This was borrowed from the Colorbox module
 *
 * @return string
 *   The JS filename for iToggle plugin.
 *
 * @link http://drupal.org/project/colorbox
 */
function itoggle_get_js() {
  $library_path = itoggle_get_path();

  // Determine library version (normal or compressed).
  $itoggle_js_map = array('none' => 'engage.itoggle.js', 'min' => 'engage.itoggle-min.js');
  $itoggle_js = $itoggle_js_map[variable_get('itoggle_compression_type', 'min')];

  if (itoggle_check_plugin_path($library_path, $itoggle_js)) {
    return "{$library_path}/{$itoggle_js}";
  }
}

/**
 * Return the JS filename for jQuery Easing plugin.
 * This was borrowed from the Colorbox module
 *
 * @return string
 *   The JS filename for jQuery Easing plugin
 *
 * @link http://drupal.org/project/colorbox
 */
function itoggle_get_easing_js() {
  $library_path = itoggle_get_easing_path();

  if (itoggle_check_easing_plugin_path($library_path)) {
    return "{$library_path}/jquery.easing.1.3.js";
  }
}

/**
 * Check if iToggle plugin exists in path.
 *
 * @param string
 *   A specific path to look in.
 * @param string
 *   A specific filename to check for.
 * @return boolean
 *   Whether the plugin exists or not.
 */
function itoggle_check_plugin_path($path = NULL, $itoggle_js = NULL) {
  if (!isset($path)) {
    $path = itoggle_get_path();
  }

  return isset($itoggle_js) ? file_exists($path . '/' . $itoggle_js) : file_exists($path . '/engage.itoggle.js') || file_exists($path . '/engage.itoggle-min.js');
}

/**
 * Check if jQuery Easing plugin exists in path.
 *
 * @param string
 *   A specific path to look in.
 * @return boolean
 *   Whether the plugin exists or not.
 */
function itoggle_check_easing_plugin_path($path = NULL) {
  if (!isset($path)) {
    $path = itoggle_get_easing_path();
  }

  return file_exists($path . '/jquery.easing.1.3.js');
}

/**
 * Return the path to the iToggle plugin.
 * This was borrowed from the Colorbox module.
 *
 * @return string
 *   The path to the iToggle library.
 *
 * @link http://drupal.org/project/colorbox
 */
function itoggle_get_path() {
  static $library_path = NULL;

  // Try to locate the library path in any possible setup.
  if (!isset($library_path)) {
    // First check the default location.
    if (is_dir(ITOGGLE_PATH)) {
      $library_path = ITOGGLE_PATH;
    }
    // Ask the libraries module as a fallback.
    elseif (module_exists('libraries')) {
      if ($path = libraries_get_path('engage.itoggle')) {
        $library_path = $path;
      }
    }
    // If no path is found fallback to the default one.
    else {
      $library_path = ITOGGLE_PATH;
    }
  }

  return $library_path;
}

/**
 * Return the path to the jQuery Easing plugin.
 * This was borrowed from the Colorbox module.
 *
 * @return string
 *   The path to the jQuery Easing library.
 *
 * @link http://drupal.org/project/colorbox
 */
function itoggle_get_easing_path() {
  static $library_path = NULL;

  // Try to locate the library path in any possible setup.
  if (!isset($library_path)) {
    // First check the default location.
    if (is_dir(ITOGGLE_EASING_PATH)) {
      $library_path = ITOGGLE_EASING_PATH;
    }
    // Ask the libraries module as a fallback.
    elseif (module_exists('libraries')) {
      if ($path = libraries_get_path('jquery.easing')) {
        $library_path = $path;
      }
    }
    // If no path is found fallback to the default one.
    else {
      $library_path = ITOGGLE_EASING_PATH;
    }
  }

  return $library_path;
}

/**
 * Return the version of iToggle plugin that is installed.
 *
 * This can be used by other modules' hook_requirements() to ensure that the
 * proper version of the iToggle plugin is installed.
 *
 * This was borrowed from the Colorbox module.
 *
 * @return string
 *   The version of the iToggle plugin.
 *
 * @see version_compare()
 * @link http://drupal.org/project/colorbox
 */
function itoggle_get_version($itoggle_js = NULL) {
  $version = 0;
  $pattern = '#Version: ([0-9\.a-z]+)#';

  // No file is passed in so use the default location.
  if (is_null($itoggle_js)) {
    $itoggle_js = itoggle_get_js();
  }

  // Return the version of iToggle plugin, it it exists.
  if (file_exists($itoggle_js)) {
    $itoggle_plugin = file_get_contents($itoggle_js, NULL, NULL, 0, 256);

    if (preg_match($pattern, $itoggle_plugin, $matches)) {
      $version = $matches[1];
    }
  }

  return $version;
}

/**
 * Return the version of jQuery Easing plugin that is installed.
 *
 * This can be used by other modules' hook_requirements() to ensure that the
 * proper version of the jQuery Easing plugin is installed.
 *
 * This was borrowed from the Colorbox module.
 *
 * @return string
 *   The version of the jQuery Easing plugin.
 *
 * @see version_compare()
 * @link http://drupal.org/project/colorbox
 */
function itoggle_get_easing_version($itoggle_easing_js = NULL) {
  $version = 0;
  $pattern = '#v([0-9\.a-z]+)#';

  // No file is passed in so use the default location.
  if (is_null($itoggle_easing_js)) {
    $itoggle_easing_js = itoggle_get_easing_js();
  }

  // Return the version of iToggle plugin, it it exists.
  if (file_exists($itoggle_easing_js)) {
    $itoggle_easing_plugin = file_get_contents($itoggle_easing_js, NULL, NULL, 0, 50);

    if (preg_match($pattern, $itoggle_easing_plugin, $matches)) {
      $version = $matches[1];
    }
  }

  return $version;
}

/**
 * Loads information about defined entities and returns an array of possible
 * boolean fields for toggling.
 *
 * @param $reset
 *   A boolean indicating whether to bypass the cache.
 * @return array
 *   An array of available entity information.
 */
function itoggle_get_entity_info($reset = FALSE) {
  // Use the advanced drupal_static() pattern, since this is called often.
  static $drupal_static_fast;

  if (!isset($drupal_static_fast)) {
    $drupal_static_fast['itoggle_get_entity_info'] = &drupal_static(__FUNCTION__);
  }

  $allowed = &$drupal_static_fast['itoggle_get_entity_info'];

  if ($reset || !is_array($allowed)) {
    $cache = cache_get('itoggle_get_entity_info');

    if (!$reset && isset($cache->data)) {
      $allowed = $cache->data;
    }
    else {
      $entities = entity_get_info();
      $allowed = array();

      foreach ($entities as $type => $info) {
        $keys = array();
        // Get table schema.
        $schema = drupal_get_schema($info['base table']);
        // Get entity metadata wrapper.
        $wrapper = entity_metadata_wrapper($type);

        foreach ($schema['fields'] as $cid => $val) {
          // Boolean fields get stored as integers.
          if ($val['type'] === 'int') {
            // Avoid fields we know for a fact aren't boolean.
            $avoid = array(
              'created',
              'changed',
              'filesize',
              'timestamp',
              'translate',
              'weight'
            );

            // Avoid more specific fields we know aren't boolean.
            if ($type === 'user') {
              $avoid[] = 'access';
              $avoid[] = 'login';
              $avoid[] = 'picture';
            }
            else if ($type === 'node') {
              $avoid[] = 'comment';
            }
            else if ($type === 'taxonomy_vocabulary') {
              $avoid[] = 'hierarchy';
            }

            // Proceed if field isn't in avoid list.
            if (!in_array($cid, $avoid)) {
              // Find field length and whether it contains the string 'id'.
              $pos = strpos($cid, 'id');
              $len = strlen($cid);

              // Avoid fields ending in 'id'.
              if ($pos === FALSE || ($len - $pos > 2)) {
                // Try to load property info from entity metadata wrapper.
                try {
                  $property = $wrapper->getPropertyInfo($cid);

                  // If we have a label, use it.
                  if (isset($property['label'])) {
                    $label = $property['label'];
                  }
                  else {
                    $label = drupal_ucfirst($cid);
                  }

                  // Boolean properties = win.
                  if ($property['type'] !== 'boolean') {
                    // If not boolean, check whether it has an options list.
                    if (isset($property['options list'])) {
                      // If options list has 2 entries, consider it a boolean.
                      $options_list = call_user_func($property['options list']);
                      if (count($options_list) !== 2) {
                        // No go, skip to next field.
                        continue;
                      }
                    }
                    else {
                      // No go, skip to next field.
                      continue;
                    }
                  }
                  $keys[$cid] = $label;
                }
                catch (EntityMetadataWrapperException $e) {
                  // If we can't use property info, take a guess.
                  $keys[$cid] = drupal_ucfirst($cid);
                }
              }
            }
          }
        }

        if (!empty($keys)) {
          $allowed[$type] = array(
            'properties' => $keys,
            'base table' => $info['base table'],
            'entity keys' => $info['entity keys'],
            'label' => $info['label'],
          );

          if (isset($info['access callback'])) {
            $allowed[$type]['access callback'] = $info['access callback'];
          }
        }
      }

      cache_set('itoggle_get_entity_info', $allowed, 'cache', CACHE_TEMPORARY);
    }
  }

  return $allowed;
}

/**
 * Return form markup used for iToggle Widget Options.
 * This is used in the Field Formatter, Field Widget and Views Handler.
 *
 * @param bool
 *   Default $clickable value.
 * @param bool
 *   Default $display_type value.
 * @param bool
 *   Whether we are overriding default settings.
 *   This is only used by the Field Formatter.
 * @return array
 *   A Form API array of fields.
 */
function itoggle_get_options_form($clickable, $display_type, $override = FALSE) {
  $form = array(
    'clickable' => array(
      '#type' => 'checkbox',
      '#title' => t('Make Widget Clickable'),
      '#description' => t("Check this box to make the iToggle Widget clickable. When clicked the widget will try to update the field's value, if the current user has the correct <a href=\"@url\">permissions</a>.", array('@url' => url('admin/people/permissions/#module-itoggle'))),
      '#default_value' => $clickable,
      '#weight' => -2,
    ),
    'display_type' => array(
      '#type' => 'select',
      '#title' => t('Display Type'),
      '#description' => t('Choose between the default On/Off, Yes/No caption or a language-agnostic 1/0 caption for the Widget.'),
      '#options' => array(t('On/Off'), t('Yes/No'), '1/0'),
      '#default_value' => $display_type,
      '#weight' => -1,
    ),
  );

  if ($override) {
    $form['override'] = array(
      '#type' => 'checkbox',
      '#title' => t('Override Widget Settings'),
      '#description' => t('Check this box to override the Widget settings when displaying this field.'),
      '#default_value' => $override,
      '#weight' => -3
    );
  }

  return $form;
}

/**
 * After build callback.
 *
 * This is used to hide the original checkbox when using an iToggle widget
 * replacement for node property fields or in the iToggle widget settings
 * form.
 *
 * @see itoggle_form_alter()
 */
function itoggle_hide_checkbox($element) {
  // Hide original checkbox.
  // We hide it like this so it can still be checked and unchecked by
  // javascript. This allows for a quick & clean way of persisting the
  // iToggle widget state.
  $element['#theme_wrappers'] = array();
  $element['#title'] = NULL;
  $element['#attributes']['style'] = array('display:none;');
  return $element;
}