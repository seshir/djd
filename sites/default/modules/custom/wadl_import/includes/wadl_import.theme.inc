<?php

/**
 * Implements hook_theme().
 */
function wadl_import_theme($existing, $type, $theme, $path) {
  return array(
    'wadl_import_theme_name' => array(
      'render element' => 'element'
    ),
    'wadl_import_theme_listing' => array(
      'render element' => 'element'
    ),
  );
}

/**
 * Implements theme_hook_theme()
 */
function theme_wadl_import_theme_name($vars) {
  $element = $vars['element'];
  drupal_add_tabledrag('parameter_table', 'order', 'sibling', 'rank-weight'); // needed for table dragging

  $header = array(
    'parameter' => t('Parameter'),
    'required' => t('Required'),
    'type' => t('Type'),
    'style' => t('Style'),
    'default' => t('Default'),
    'description' => t('Description'),
    'weight' => t('Rank'),
  );

  $rows = array();
  foreach (element_children($element) as $key) {
    $row = array();

    $row['data'] = array();
    foreach ($header as $fieldname => $title) {
      $row['data'][] = drupal_render($element[$key][$fieldname]);
      $row['class'] = array('draggable'); // needed for table dragging
    }
    $rows[] = $row;
  }

  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'parameter_table'), // needed for table dragging
  ));
}

/**
 * Implements theme_hook_theme()
 */
function theme_wadl_import_theme_listing($vars) {

  $element = $vars['element'];

  $header = array(
    'title' => t('Display Name'),
    'name' => t('Method'),
    'description' => t('Description'),
    'term' => t('Term'),
    'import' => t('Import'),
  );

  $rows = array();
  foreach (element_children($element) as $key) {
    $row = array();

    $row['data'] = array();
    foreach ($header as $fieldname => $title) {
      $row['data'][] = drupal_render($element[$key][$fieldname]);
    }
    $rows[] = $row;
  }

  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
  ));
}



/**
 * Implements hook_views_pre_render().
 *
 * This hook could be costly, however it fins the field_parameters field, and renders it out of it's serialized state.
 *
 */
function wadl_import_views_pre_render(&$view) {
  $results = &$view->result;
  foreach ($results as $key => $result) {
    $parameters = NULL;

    if(isset($results[$key]->field_field_parameters[0]['rendered']['#markup'])) {
      $parameters = drupal_json_decode($results[$key]->field_field_parameters[0]['rendered']['#markup']);
    }
    if (!empty($parameters)) {
      $header = array(
        'parameter' => t('Parameter'),
        'required' => t('Required'),
        'type' => t('Type'),
        'style' => t('Style'),
        'default' => t('Default'),
        'description' => t('Description'),
      );
      $rows = array();
      $api_info = drupal_json_decode($results[$key]->field_field_parameters[0]['rendered']['#markup']);
      foreach($api_info as $data) {
        $rows[] = array($data['parameter'], $data['required'], $data['type'], $data['style'], $data['default'], $data['description']);
      }
      $results[$key]->field_field_parameters[0]['rendered']['#markup'] = theme('table', array('header' => $header,'rows'=> $rows));
    } else {
      $results[$key]->field_field_parameters[0]['rendered']['#markup'] = '';
    }
  }
}


