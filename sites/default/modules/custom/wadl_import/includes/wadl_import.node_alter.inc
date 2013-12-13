<?php

/**
 * Implements hook_form_alter()
 */
function wadl_import_form_alter(&$form, &$form_state, $form_id) {
  if ($form_id == 'api_resource_doc_node_form') {
    $form['field_parameters']['und'][0]['value']['#type'] = 'hidden';
    if(is_null($form['field_parameters']['und'][0]['value']['#default_value'])) {
      // needs to be a string in order in order for Drupal to render a value, so we'll encode an empty array
      $form['field_parameters']['und'][0]['value']['#default_value'] = drupal_json_encode(array());
    }
    $parameters = drupal_json_decode($form['field_parameters']['und'][0]['value']['#default_value']);
    $form['parameters'] = array(
      '#prefix' => '<div id="api-parameters">',
      '#suffix' => '</div>',
      '#tree' => TRUE,
      '#theme' => 'wadl_import_theme_name',
      '#weight' => $form['field_parameters']['#weight'],
    );
    $api_info = array();
    $i = 0;
    if (!empty($parameters)) {
      foreach($parameters as $key => $characters) {
        $api_info[] = array(
          'parameter' => $characters['parameter'],
          'required' => $characters['required'],
          'type' => $characters['type'],
          'style' => $characters['style'],
          'default' => $characters['default'],
          'description' => $characters['description'],
          'rank' => $i,
        );
        $i++;
      }
    } else {
      $api_info[] = array(
        'parameter' => '',
        'required' => '',
        'type' => '',
        'style' => '',
        'default' => '',
        'description' => '',
        'rank' => 0,
      );
    }
    if (isset($form_state['storage']['ajaxed'])) {
      $count = count($api_info);
      if ($form_state['storage']['num_parameters'] > $count) {
        $add = $form_state['storage']['num_parameters'] - $count;
        for ($i = 0; $i < $add; $i++) {
          $api_info[] = array(
            'parameter' => '',
            'required' => '',
            'type' => '',
            'style' => '',
            'default' => '',
            'description' => '',
            'rank' => count($api_info) + 1,
          );
        }
      }
      if ($form_state['storage']['num_parameters'] < $count) {
        for ($i = $form_state['storage']['num_parameters']; $i < $count; $i++) {
          unset($api_info[$i]);
        }
      }
    }
    foreach($api_info as $key => $characters) {
      $form['parameters'][$key]['parameter'] = array(
        '#type' => 'textfield',
        '#default_value' => $characters['parameter'],
        '#size' => 30,
      );
      $form['parameters'][$key]['required'] = array(
        '#type' => 'textfield',
        '#default_value' => $characters['required'],
        '#size' => 30,
      );
      $form['parameters'][$key]['type'] = array(
        '#type' => 'textfield',
        '#default_value' => $characters['type'],
        '#size' => 30,
      );
      $form['parameters'][$key]['style'] = array(
        '#type' => 'textfield',
        '#default_value' => $characters['style'],
        '#size' => 30,
      );
      $form['parameters'][$key]['default'] = array(
        '#type' => 'textfield',
        '#default_value' => $characters['default'],
        '#size' => 30,
      );
      $form['parameters'][$key]['description'] = array(
        '#type' => 'textarea',
        '#default_value' => $characters['description'],
      );
      $form['parameters'][$key]['weight'] = array(
        '#type' => 'textfield',
        '#default_value' => $characters['rank'],
        '#size' => 3,
        '#attributes' => array(
          'class' => array(
            'rank-weight'
          )
        ),
      );
    }
    $form['add_parameter'] = array(
      '#type' => 'submit',
      '#value' => t('Add Parameter'),
      '#weight' => $form['field_parameters']['#weight'] +1,
      '#submit' => array('wadl_import_add_parameter_node_add_one'),
      '#ajax' => array(
        'callback' => 'wadl_import_add_parameter_node_callback',
        'wrapper' => 'api-parameters',
        'progress' => array(
          'type' => 'none'
        )
      ),
      '#limit_validation_errors' => array(array('title')),
      '#attributes' => array(
        'class' => array(
          'add-parameter'
        )
      ),
    );
    $form['remove_parameter'] = array(
      '#type' => 'submit',
      '#value' => t('Remove Parameter'),
      '#weight' => $form['field_parameters']['#weight'] +1,
      '#submit' => array('wadl_import_remove_parameter_node_remove_one'),
      '#ajax' => array(
        'callback' => 'wadl_import_remove_parameter_node_callback',
        'wrapper' => 'api-parameters',
        'progress' => array(
          'type' => 'none'
        )
      ),
      '#limit_validation_errors' => array(array('title')),
      '#attributes' => array(
        'class' => array(
          'add-parameter'
        )
      ),
    );
  }
}

/**
 * Submit callback for the add parameter function
 */
function wadl_import_add_parameter_node_add_one($form, &$form_state) {
  $form_state['storage']['ajaxed'] = TRUE;
  $keys = array_keys($form['parameters']);
  $count = 0;
  foreach($keys as $value) {
    if (is_numeric($value)) {
      $count++;
    }
  }
  $form_state['storage']['num_parameters'] = $count;
  $form_state['storage']['num_parameters']++;
  $form_state['rebuild'] = TRUE;
}

/**
 * Submit Callback for the add parameter function
 */
function wadl_import_add_parameter_node_callback($form, $form_state) {
  return $form['parameters'];
}

/**
 * Submit callback for the remove parameter function
 */
function wadl_import_remove_parameter_node_remove_one($form, &$form_state) {
  $form_state['storage']['ajaxed'] = TRUE;
  if (isset($form_state['storage']['num_parameters'])) {
    if ($form_state['storage']['num_parameters'] > 1) {
      $form_state['storage']['num_parameters']--;
    }
  } else {
    $keys = array_keys($form['parameters']);
    $count = 0;
    foreach($keys as $value) {
      if (is_numeric($value)) {
        $count++;
      }
    }
    $form_state['storage']['num_parameters'] = $count;
    if ($count > 1) {
      $form_state['storage']['num_parameters']--;
    }
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * Submit Callback for the add parameter function
 */
function wadl_import_remove_parameter_node_callback($form, $form_state) {
  return $form['parameters'];
}

/**
 * Implements hook_node_validate()
 */
function wadl_import_node_validate($node, $form, &$form_state) {
  if ($node->type == 'api_resource_doc') {
    $parameters = array();
    foreach($form_state['values']['parameters'] as $params) {
      if (!empty($params['parameter'])) {
        $parameters[$params['weight']] = $params;
      }
    }
    uksort($parameters, 'strcasecmp');
    $form_state['values']['field_parameters']['und'][0]['value'] = drupal_json_encode($parameters);
  }
}

/**
 * Implements preprocess_node()
 */
function wadl_import_preprocess_node(&$vars) {
  if (isset($vars['field_parameters'][0]['value'])) {
    $parameters = drupal_json_decode($vars['field_parameters'][0]['value']);
    if (!empty($parameters)) {
      $header = array('Parameter', 'Required', 'Type','Style', 'Default', 'Description');
      $rows = array();
      $params = drupal_json_decode($vars['field_parameters'][0]['value']);
      foreach ($params as $data) {
        $rows[] = array($data['parameter'], $data['required'], $data['type'], $data['style'], $data['default'], $data['description']);
      }
      $vars['content']['field_parameters'][0]['#markup'] = theme('table', array('header' => $header,'rows'=> $rows));
    } else {
      $vars['content']['field_parameters'][0]['#markup'] = '';
    }
  }

}