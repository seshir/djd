<?php
/**
 * @file
 * swisscom_api_documentation.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function swisscom_api_documentation_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "field_group" && $api == "field_group") {
    return array("version" => "1");
  }
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_image_default_styles().
 */
function swisscom_api_documentation_image_default_styles() {
  $styles = array();

  // Exported image style: icon.
  $styles['icon'] = array(
    'name' => 'icon',
    'effects' => array(
      1 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => '32',
          'height' => '32',
        ),
        'weight' => '1',
      ),
    ),
  );

  return $styles;
}

/**
 * Implements hook_node_info().
 */
function swisscom_api_documentation_node_info() {
  $items = array(
    'api_console' => array(
      'name' => t('API Console'),
      'base' => 'node_content',
      'description' => t('This content type is for creating API Consoles. '),
      'has_title' => '1',
      'title_label' => t('API Console Name'),
      'help' => t('The API Console content type is used for creating API Consoles.  When you create an API Console Node, it will be displayed in the list of API Consoles to the user.'),
    ),
    'api_product' => array(
      'name' => t('API Product'),
      'base' => 'node_content',
      'description' => t('API Product retrieved from Management server'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
    'api_resource_doc' => array(
      'name' => t('API Method'),
      'base' => 'node_content',
      'description' => t('Create documentation for API methods.'),
      'has_title' => '1',
      'title_label' => t('API Name'),
      'help' => '',
    ),
    'page' => array(
      'name' => t('Basic page'),
      'base' => 'node_content',
      'description' => t('Use <em>basic pages</em> for your static content, such as an \'About us\' page.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}

/**
 * Implements hook_weight_features_default_settings().
 */
function swisscom_api_documentation_weight_features_default_settings() {
  $settings = array();

  $settings['api_resource_doc'] = array(
    'enabled' => '1',
    'range' => '100',
    'menu_weight' => '0',
    'default' => '0',
    'sync_translations' => '1',
  );

  return $settings;
}
