<?php

/**
 * Form that allows the import of WADL Files
 *
 * @description:
 * Drupal form that uploads an XML file that can be used to import methods as nodes.
 *
 * @params:
 * $form: Array of form data.
 * $form_state: Array of current form state data.
 *
 */
function wadl_import_wadl_form($form, $form_state) {

  $form = array();

  if (isset($form_state['storage']['method'])) {
    if (!$form_state['storage']['method_complete']) {
      return wadl_import_wadl_node_create($form, $form_state);
    }
  }
  else {
    if (isset($form_state['storage']['listing'])) {
      return wadl_import_wadl_node_listing($form, $form_state);
    }
  }

  // Get node content types for content type options.
  $types = node_type_get_types();
  foreach ($types as $type) {
    $options[$type->type] = $type->name;
  }

  $_SESSION['api_docbuilder_xmlstring'] = NULL;

  $form['content_type'] = array(
    '#type' => 'select',
    '#title' => t('Content Type'),
    '#description' => 'All methods will be imported as nodes of this content type.',
    '#options' => (empty($options)) ? array() : $options,
    '#default_value' => 'api_resource_doc',
    '#required' => TRUE,
  );

  // Get taxonomy name list to have user select API Name
  $taxonomy_names = taxonomy_vocabulary_get_names();
  $options_api_name_vocab =  array('0' => '-- Select a Value --');
  foreach ($taxonomy_names as $taxonomy_name) {
    $options_api_name_vocab[$taxonomy_name->vid] = $taxonomy_name->name;
  }

  // If we have a value for the first dropdown from $form_state['values'] we use
  // this both as the default value for the first dropdown and also as a
  // parameter to pass to the function that retrieves the options for the
  // second dropdown.
  $selected_api_name_vocab = isset($form_state['values']['api_name_vocab']) ? $form_state['values']['api_name_vocab'] : 0;


  // If the $selected_api_name_vocab is not set, use what they used last time.
  if($selected_api_name_vocab == 0){
    $selected_api_name_vocab = variable_get('wadl_import_api_name_vocab_default',0);
  }

  $form['api_name_vocab'] = array(
    '#type' => 'select',
    '#title' => t('API Name Vocabulary'),
    '#description' => 'The API Name taxomomy that is used to tag API methods.',
    '#options' => (empty($options_api_name_vocab)) ? array() : $options_api_name_vocab,
    '#default_value' => $selected_api_name_vocab,
    '#required' => TRUE,
    '#ajax' => array(
      // When 'event' occurs, Drupal will perform an ajax request in the
      // background.
      'callback' => 'wadl_import_api_name_callback',
      'wrapper' => 'dropdown-api-name-term-replace',
    ),
  );

  if($selected_api_name_vocab != 0) {
    $api_name_term_title = $options_api_name_vocab[$selected_api_name_vocab] . ' ' . t('Term');
  }
  else {
    $api_name_term_title = 'API Name Term';
  }

  $form['api_name_term'] = array(
    '#type' => 'select',
    '#title' => $api_name_term_title,
    // The entire enclosing div created here gets replaced when dropdown_first
    // is changed.
    '#prefix' => '<div id="dropdown-api-name-term-replace">',
    '#suffix' => '</div>',
    // when the form is rebuilt during ajax processing, the $selected variable
    // will now have the new value and so the options will change
    '#options' => _wadl_import_get_api_name_term_options($selected_api_name_vocab),
    '#default_value' => isset($form_state['values']['dropdown_second']) ? $form_state['values']['dropdown_second'] : 0,
  );

  $form['upload'] = array(
    '#type' => 'fieldset',
    '#title' => t('Upload file'),
    '#collapsible' => FALSE,
  );
  $form['upload']['file'] = array(
    '#type' => 'file',
    '#description' => t('WADL file that contains XML for the data import.')
  );
  $form['next'] = array(
    '#type' => 'submit',
    '#value' => t('Upload'),
  );

  return $form;
}

/**
 * Selects just the second dropdown to be returned for re-rendering
 *
 * Since the controlling logic for populating the form is in the form builder
 * function, all we do here is select the element and return it to be updated.
 *
 * @return renderable array (the second dropdown)
 */
function wadl_import_api_name_callback($form, $form_state) {
  variable_set('wadl_import_api_name_vocab_default',$form_state['values']['api_name_vocab']);
  return $form['api_name_term'];
}

function _wadl_import_get_api_name_term_options($vid) {
  // drupal_map_assoc() just makes an array('String' => 'String'...).
  $terms = taxonomy_term_load_multiple(array(), array('vid' => $vid));
  $options = array('0' => '-- Select a Value --');
  foreach ($terms as $term) {
    $options[$term->tid] = $term->name;
  }
  return $options;

}

/**
 * Step 2 of the WADL Import Form
 */
function wadl_import_wadl_node_listing($form, $form_state) {

  $api_info = array();
  $xml_string = wadl_import_get_xmlstring($form_state);
  $methods = wadl_import_wadl_import($xml_string);

  $i = 0;
  foreach ($methods as $method) {

    $title = (!empty($method->displayName)) ? $method->displayName : $method->id;
    $import = TRUE;
    $result = db_select('node', 'ti')
      ->fields('ti', array('title'))
      ->condition('title', $title)
      ->execute()
      ->rowCount();
    if ($result) {
      $import = FALSE;
    }
    $api_info[] = array(
      'title' => $title,
      'name' => $method->name,
      'description' => $method->doc,
      'term' => $method->tag,
      'rank' => $i,
      'import' => $import,
      'id' => $method->id,
    );
    $i++;
  }
  $form['import_method'] = array(
    '#type' => 'select',
    '#title' => t('Data Import Method:'),
    '#options' => array(
      'manual' => 'Manual Entry',
      'automatic' => 'Automatic Entry',
    ),
    '#default_value' => array('manual'),
  );
  $form['methods'] = array(
    '#prefix' => '<div id="api-methods">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
    '#theme' => 'wadl_import_theme_listing'
  );
  if (isset($api_info[0]['id'])) {
    $form['method_id'] = array(
      '#type' => 'hidden',
      '#default_value' => $api_info[0]['id'],
    );
  }
  foreach ($api_info as $key => $characters) {
    $form['methods'][$key]['title'] = array(
      '#type' => 'markup',
      '#markup' => $characters['title'],
    );
    $form['methods'][$key]['name'] = array(
      '#type' => 'markup',
      '#markup' => $characters['name'],
    );
    $form['methods'][$key]['description'] = array(
      '#type' => 'markup',
      '#markup' => $characters['description'],
    );
    $form['methods'][$key]['term'] = array(
      '#type' => 'markup',
      '#markup' => (!empty($characters['term']) ? $characters['term'] : $characters['title']),
    );
    $form['methods'][$key]['weight'] = array(
      '#type' => 'hidden',
      '#default_value' => $characters['rank'],
      '#size' => 3,
      '#attributes' => array(
        'class' => array(
          'rank-weight'
        )
      ),
    );
    $form['methods'][$key]['import'] = array(
      '#type' => 'checkbox',
      '#default_value' => (bool)$characters['import']
    );
    $form['methods'][$key]['id'] = array(
      '#type' => 'hidden',
      '#default_value' => $characters['id'],
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save & Continue'),
  );

  return $form;
}

/**
 * Step 3 of the WADL Import Form
 */
function wadl_import_wadl_node_create($form, $form_state) {

  $form['#tree'] = TRUE;
  $xml_string = wadl_import_get_xmlstring($form_state);
  $methods = wadl_import_wadl_import($xml_string);

  $array = array_values($methods);
  $key = $array[$form_state['storage']['method_count']]->id;

  $display_name = $methods[$key]->displayName;
  $key_id = $methods[$key]->id;
  $api_info = array();

  $form['title_' . $key_id] = array(
    '#type' => 'textfield',
    '#title' => t('API Title'),
    '#default_value' => (!empty($display_name)) ? $display_name : $key_id,
    '#size' => 60,
    '#required' => TRUE,
  );
  $form['description_' . $key_id] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#default_value' => $methods[$key]->doc,
    '#size' => 60,
  );
  $options = array('GET' => t('GET'), 'POST' => t('POST'),'PUT' => t('PUT'), 'DELETE' => t('DELETE') );
  $form['method_name_' . $key_id] = array(
      '#type' => 'radios',
      '#title' => t('Method Name'),
      '#default_value' => isset($methods[$key]->name) ? $methods[$key]->name : NULL,
      '#options' => $options,
  );

  $form['path_' . $key_id] = array(
    '#type' => 'textfield',
    '#title' => t('Path'),
    '#default_value' => $methods[$key]->base_url . $methods[$key]->path,
    '#size' => 60,
  );
  foreach ($methods[$key]->parameters as $object) {
    if (strpos($object->default,'json') !== false) {
      $json = TRUE;
    }
    if (strpos($object->default,'xml') !== false) {
      $xml = TRUE;
    }
  }
  if (!empty($methods[$key]->parameters)) {
    $i = 0;
    foreach($methods[$key]->parameters as $element => $params) {
      if ($element != 'format') {
        $api_info[] = array(
          'parameter' => $methods[$key]->parameters[$element]->name,
          'required' => $methods[$key]->parameters[$element]->required,
          'type' => $methods[$key]->parameters[$element]->type,
          'style' => $methods[$key]->parameters[$element]->style,
          'default' => $methods[$key]->parameters[$element]->default,
          'description' => $methods[$key]->parameters[$element]->doc,
          'rank' => $i,
        );
        $i++;
      }
    }
    if (isset($form_state['storage']['ajaxed'])) {
      if (isset($form_state['storage']['num_parameters'])) {
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
    }
  }
  if (!empty($api_info)) {
    $form['json_' . $key_id] = array(
      '#type' => 'hidden',
      '#default_value' => '',
      '#attributes' => array(
        'class' => array(
          'hidden_json'
        )
      )
    );
    $form['parameters_' . $key_id] = array(
      '#prefix' => '<div id="api-parameters">',
      '#suffix' => '</div>',
      '#tree' => TRUE,
      '#theme' => 'wadl_import_theme_name'
    );
    foreach ($api_info as $element => $characters) {
      $form['parameters_' . $key_id][$element]['parameter'] = array(
        '#type' => 'textfield',
        '#default_value' => $characters['parameter'],
        '#size' => 30,
      );
      $form['parameters_' . $key_id][$element]['required'] = array(
        '#type' => 'textfield',
        '#default_value' => $characters['required'],
        '#size' => 30,
      );
      $form['parameters_' . $key_id][$element]['type'] = array(
        '#type' => 'textfield',
        '#default_value' => $characters['type'],
        '#size' => 30,
      );
      $form['parameters_' . $key_id][$element]['style'] = array(
        '#type' => 'textfield',
        '#default_value' => $characters['style'],
        '#size' => 30,
      );
      $form['parameters_' . $key_id][$element]['default'] = array(
        '#type' => 'textfield',
        '#default_value' => $characters['default'],
        '#size' => 30,
      );
      $form['parameters_' . $key_id][$element]['description'] = array(
        '#type' => 'textarea',
        '#default_value' => $characters['description'],
      );
      $form['parameters_' . $key_id][$element]['weight'] = array(
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
      '#submit' => array('wadl_import_add_parameter_add_one'),
      '#ajax' => array(
        'callback' => 'wadl_import_add_parameter_callback',
        'wrapper' => 'api-parameters',
        'progress' => array(
          'type' => 'none'
        )
      ),
      '#attributes' => array(
        'class' => array(
          'add-parameter'
        )
      ),
    );
    $form['remove_parameter'] = array(
      '#type' => 'submit',
      '#value' => t('Remove Parameter'),
      '#submit' => array('wadl_import_remove_parameter_remove_one'),
      '#ajax' => array(
        'callback' => 'wadl_import_remove_parameter_callback',
        'wrapper' => 'api-parameters',
        'progress' => array(
          'type' => 'none'
        )
      ),
      '#attributes' => array(
        'class' => array(
          'add-parameter'
        )
      ),
    );
  }
  $form['parameter_table_break'] = array(
    '#type' => 'markup',
    '#prefix' => '<br><br>',
  );
  if (isset($methods[$key]->tag)) {
    $form['tag_' . $key_id] = array(
      '#type' => 'select',
      '#title' => t('API Tag'),
      '#options' => array(
        $methods[$key]->tag => $methods[$key]->tag,
      ),
      '#default_value' => $methods[$key]->tag,
    );
  } else {
    $tids = array();
    $vocabularies = taxonomy_get_vocabularies();
    foreach($vocabularies as $vocab) {
      if ($vocab->machine_name == 'api_tag') {
        $terms = taxonomy_get_tree($vocab->vid);
        foreach($terms as $term) {
          $tids[$term->name] = $term->name;
        }
      }
    }
    $form['tag_' . $key_id] = array(
      '#type' => 'select',
      '#title' => t('API Tag'),
      '#options' => $tids,
    );
  }

  $form['method_id'] =  array(
    '#type' => 'hidden',
    '#value' => $key,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#executes_submit_callback' => TRUE,
    '#value' => t('Save & Continue'),
  );

  return $form;
}

/**
 * Submit callback for the add parameter function
 */
function wadl_import_add_parameter_add_one($form, &$form_state) {
  $method_id = $form_state['values']['method_id'];
  $form_state['storage']['num_parameters'] = count($form_state['values']['parameters_' . $method_id]);
  $form_state['storage']['num_parameters']++;
  $form_state['storage']['ajaxed'] = TRUE;
  $form_state['rebuild'] = TRUE;
}

/**
 * AJAX Callback for the add parameter function
 */
function wadl_import_add_parameter_callback($form, $form_state) {
  $method_id = $form_state['values']['method_id'];
  return $form['parameters_' . $method_id];
}

/**
 * Submit callback for the remove parameter function
 */
function wadl_import_remove_parameter_remove_one($form, &$form_state) {
  $form_state['storage']['ajaxed'] = TRUE;
  $method_id = $form_state['values']['method_id'];
  if (isset($form_state['storage']['num_parameters'])) {
    if ($form_state['storage']['num_parameters'] > 1) {
      $form_state['storage']['num_parameters']--;
    }
  } else {
    $count = count($form_state['values']['parameters_' . $method_id]);
    $form_state['storage']['num_parameters'] = count($form_state['values']['parameters_' . $method_id]);
    if ($count > 1) {
      $form_state['storage']['num_parameters']--;
    }
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * AJAX Callback for the remove parameter function
 */
function wadl_import_remove_parameter_callback($form, $form_state) {
  $method_id = $form_state['values']['method_id'];
  return $form['parameters_' . $method_id];
}

/**
 * Validate function for import form
 */
function wadl_import_wadl_form_validate($form, &$form_state) {
  if($form_state['triggering_element']['#name'] = 'api_name_vocab') {
    return;
  }

  if (!isset($form_state['storage']['listing'])) {
    if (!$_FILES['files']['name']['file'] && !$form_state['values']['code']) {
      drupal_set_message(t('Please upload a file to import.'), 'error');
      form_set_error('file', NULL);
    }
  }
}

/**
 * Submit function for import form.
 *
 * Modifies this function so that it will respond appropriately based on
 * which page was submitted. If the first page is being submitted,
 * values in the 'storage' array are saved and the form gets
 * automatically reloaded.
 *
 * If page 2 was submitted, we display a message and redirect the
 * user to another page.
 */
function wadl_import_wadl_form_submit($form, &$form_state) {
  if (isset($form_state['values']['methods']) || isset($form_state['storage']['method'])) {
    if ((isset($form_state['values']['import_method']) && $form_state['values']['import_method'] == 'manual') ||
      (isset($form_state['storage']['import_method']) && $form_state['storage']['import_method'] == 'manual')) {
      $form_state['rebuild'] = TRUE;
      $form_state['storage']['method_complete'] = FALSE;
      $form_state['storage']['import_method'] = 'manual';
      if (!isset($form_state['storage']['method'])) {
        $form_state['storage']['method'] = $form_state['values']['methods'];
      }
      if (isset($form_state['storage']['method_count'])) {
        $method = array();
        foreach($form_state['storage']['method'] as $count => $which) {
          if ($which['import'] == 1) {
            $method[] = array(
              'weight' => $which['weight'],
              'import' => $which['import'],
              'id' => $which['id'],
            );
          }
        }
        $i = 0;
        $method_count = NULL;
        foreach($method as $find) {
          if ($i == 0) {
            $method_count = $find['weight'];
          }
          $i++;
        }
        if (is_null($method_count)) {
          $form_state['storage']['method_complete'] = TRUE;
        }
        $method[0]['import'] = 0;
        $form_state['storage']['method'] = $method;
        $form_state['storage']['method_count'] = $method_count;
        $method_id = $form_state['values']['method_id'];
        _wadl_import_node_save($form_state, $method_id);
        $form_state['storage']['ajaxed'] = FALSE;

      } else {
        $method = array();
        foreach($form_state['storage']['method'] as $count => $which) {
          if ($which['import'] == 1) {
            $method[] = array(
              'weight' => $which['weight'],
              'import' => $which['import'],
              'id' => $which['id'],
            );
          }
        }
        $method[0]['import'] = 0;
        $form_state['storage']['method'] = $method;
        $form_state['storage']['method_count'] = $method[0]['weight'];
      }
      return;
    } else {
      batch_set(
        array(
          'title' => t('Automatically Generate WADL Nodes'),
          'operations' => array(
            array('_wadl_import_automatic_batch', array($form_state)),
          ),
          'finished' => '_wadl_import_finished_batch',
        )
      );
      batch_process('admin/content/wadl_import');
    }
  }
  if (!isset($form_state['storage']['listing'])) {
    if ($form_state['clicked_button']['#id'] == 'edit-next') {
      if ($_FILES['files']['name']['file']) {
        $original = $_FILES['files']['name']['file'];
        $validation = array(
          'file_validate_extensions' => array('xml wadl'),
        );
        $directory = 'public://' . WADL_SUBDIRECTORY;
        $prepare = file_prepare_directory($directory, FILE_CREATE_DIRECTORY);
        if (!$prepare) {
          drupal_set_message(t('Error: Could not prepare the directory for the file.'), 'error');
        } else {
          $save = file_save_upload('file', $validation, $directory, FILE_EXISTS_REPLACE);
          if (!$save) {
            drupal_set_message(t('Error: Could not save the file to the directory.'), 'error');
          } else {
            $save->original = $original;
            form_set_value($form['upload']['file'], serialize($save), $form_state);
            drupal_set_message('File Uploaded Successfully', 'status');
          }
        }
      }
      $form_state['rebuild'] = TRUE;
      $form_state['storage']['listing'] = TRUE;
      $form_state['storage']['page_one_values'] = $form_state['values'];
      return;
    }
  }
}

/**
 * Create a new term for the given nodes, if it doesn't already exist
 */
function _wadl_import_create_taxonomy_term($form_state, $method_id) {
  $term_exists = FALSE;
  $vocabularies = taxonomy_get_vocabularies();
  foreach($vocabularies as $vocab) {
    if ($vocab->machine_name == 'api_tag') {
      $terms = taxonomy_get_tree($vocab->vid);
      $vid = $vocab->vid;
      foreach($terms as $term) {
        if ($term->name == $form_state['values']['tag_' . $method_id]) {
          $term_exists = TRUE;
          continue;
        }
      }
    }
  }
  if (isset($terms) && isset($vid)) {
    if (!$term_exists) {
      $newterm = new stdClass();
      $newterm->name = $form_state['values']['tag_' . $method_id];
      $newterm->vid = $vid;
      taxonomy_term_save($newterm);
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Saves a node
 */
function _wadl_import_node_save($form_state, $method_id) {
  _wadl_import_create_taxonomy_term($form_state, $method_id);

  $vocabularies = taxonomy_get_vocabularies();
  foreach($vocabularies as $vocab) {
    if ($vocab->machine_name == 'api_tag') {
      $terms = taxonomy_get_tree($vocab->vid);
      foreach($terms as $term) {
        if ($term->name == $form_state['values']['tag_' . $method_id]) {
          $tid = $term->tid;
          continue;
        }
      }
    }
  }

  $node = new stdClass();
  $node->type = $form_state['storage']['page_one_values']['content_type'];
  node_object_prepare($node);

  $node->title = $form_state['values']['title_' . $method_id];
  $node->language = LANGUAGE_NONE;
  $node->body[$node->language][0]['value'] = $form_state['values']['description_' . $method_id];
  $node->body[$node->language][0]['format'] = 'plain_text';

  $options = array('GET' => t('GET'), 'POST' => t('POST'),'PUT' => t('PUT'), 'DELETE' => t('DELETE') );

  if (isset($form_state['values']['method_name_' . $method_id])) {
    $node->field_api_method_name['und'][0]['value'] = $form_state['values']['method_name_' . $method_id];
  }

  $node->field_api_method_tag['und'][0]['tid'] = $tid;
  $node->field_path['und'][0]['value'] = $form_state['values']['path_' . $method_id];
  if (isset($form_state['values']['parameters_' . $method_id])) {
    if (empty($form_state['values']['json_' . $method_id])) {
      $node->field_parameters['und'][0]['value'] = drupal_json_encode($form_state['values']['parameters_' . $method_id]);
    } else {
      $node->field_parameters['und'][0]['value'] = $form_state['values']['json_' . $method_id];
    }
  } else {
    $node->field_parameters['und'][0]['value'] = drupal_json_encode('');
  }
  $node->field_sample_resource_xml['und'][0]['value'] = $form_state['values']['sample_' . $method_id]['sample_resource_xml_' . $method_id];
  $node->field_sample_resource_json['und'][0]['value'] = $form_state['values']['sample_' . $method_id]['sample_resource_json_' . $method_id];

  // Set API Name taxonomy term
  $node->field_api_name['und'][0]['tid'] = $form_state['storage']['page_one_values']['api_name_term'];

  node_save($node);

  $path = 'node/' . $node->nid;
  $title = $node->title;
  $content = l('View '. $title, $path);
  drupal_set_message('File has created a node successfully. ' . $content, 'status');
}