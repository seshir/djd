<?php
/**
 * @file
 * swisscom_api_documentation.field_group.inc
 */

/**
 * Implements hook_field_group_info().
 */
function swisscom_api_documentation_field_group_info() {
  $export = array();

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_api_attributes|taxonomy_term|api_name|form';
  $field_group->group_name = 'group_api_attributes';
  $field_group->entity_type = 'taxonomy_term';
  $field_group->bundle = 'api_name';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'API Attributes',
    'weight' => '2',
    'children' => array(
      0 => 'field_api_access',
      1 => 'field_api_product',
      2 => 'field_api_upcoming',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => '',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_api_attributes|taxonomy_term|api_name|form'] = $field_group;

  return $export;
}
