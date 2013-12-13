<?php
/**
 * @file
 * wadl_import_feature.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function wadl_import_feature_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_views_api().
 */
function wadl_import_feature_views_api() {
  return array("version" => "3.0");
}

/**
 * Implements hook_node_info().
 */
function wadl_import_feature_node_info() {
  $items = array(
    'api_resource_doc' => array(
      'name' => t('API Resource Doc'),
      'base' => 'node_content',
      'description' => t('The default content type for API resource documentation.'),
      'has_title' => '1',
      'title_label' => t('API Title'),
      'help' => '',
    ),
  );
  return $items;
}
