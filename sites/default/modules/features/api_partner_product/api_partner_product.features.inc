<?php
/**
 * @file
 * api_partner_product.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function api_partner_product_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_node_info().
 */
function api_partner_product_node_info() {
  $items = array(
    'api_partner_product' => array(
      'name' => t('API Partner Product'),
      'base' => 'node_content',
      'description' => t('This is to provide business overview of API products to Partners.'),
      'has_title' => '1',
      'title_label' => t('API Product'),
      'help' => '',
    ),
  );
  return $items;
}
