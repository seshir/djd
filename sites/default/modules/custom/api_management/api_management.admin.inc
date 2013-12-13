<?php

 /**
 * @file
 * API Management Edit module include file.
 *
 * @author
 * Cesar Galindo <cesar@apigee.com>
 *
 * @since 2013-01-Sept
 */


/**
 * A modal login callback.
 */
function _api_management_ajax_change_api_name($js = NULL, $name, $id, $nid) {

  // Fall back if $js is not set.
  if (!$js) {
    return drupal_get_form('api_management_change_api_name_form');
  }

  ctools_include('modal');
  ctools_include('ajax');
  $form_state = array(
    'title' => t('Change API Name - ' . $name),
    'ajax' => TRUE,
  );

  $output = ctools_modal_form_wrapper('api_management_change_api_name_form', $form_state);

  if (!empty($form_state['executed'])) {
    // We'll just overwrite the form output if it was successful.
    $output = array();
    $output[] = ctools_modal_command_dismiss();

    if (!strpos($form_state['clicked_button']['#id'], '-cancel--')) {
      $output[] = ajax_command_remove("a.api-remove-$id");
      $output[] = ajax_command_replace("tr.api-row-".$id. " .views-field-status", '<td class="views-field views-field-status red">Changed API Name</td>');
    }
  }
  print ajax_render($output);
  exit;
}

/**
 * Form constructor for deleting an API.  Built via a menu callback to drupal_get_form().
 *
 * Incoming URL: admin/api/%ctools_js/api_management_change_api_name_form/%
 *
 * @return array
 */
function api_management_change_api_name_form() {
  $form = array();

  $form['api_warning'] = array(
    '#markup' => t('Warning, you are about to change the  API title for API Console, Forums, Blogs, API Product, and Documentation.<br><br>'),
  );

  $form['api_id'] = array(
    '#type' => 'hidden',
    '#value' => arg(5),
  );

  $form['api_name'] = array(
    '#type' => 'hidden',
    '#value' => arg(4),
  );

  $form['api_nid'] = array(
    '#type' => 'hidden',
    '#value' => arg(6),
  );

  $form['current_api_name'] = array(
    '#type' => 'textfield',
    '#disabled' => TRUE,
    '#title' => t('From:'),
    '#default_value' => arg(4),
  );

  $form['api_ms_name'] = array(
    '#type' => 'textfield',
    '#title' => t('To:'),
    '#default_value' => arg(7),
  );

  $form['cancel'] = array(
    '#prefix' => '<br>',
    '#type' => 'submit',
    '#value' => 'Cancel' ,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Change API Name' ,
  );

  $form['#validate'] = array('api_management_change_api_name_form_validate');
  $form['#submit'][0] = 'api_management_change_api_name_form_submit';

  return $form;
}

/**
 * Helper handler to validate API name
 *
 * @param array $form
 * @param array $form_state
 */
function api_management_change_api_name_form_validate (&$form, &$form_state) {
  // Check if Cancel button is clicked
  if (strpos($form_state['clicked_button']['#id'], '-cancel--')) {
    return;
  }

  // Validate API Name must only contains letters, digits, spaces, dashes, and underscores.
  if (preg_match('/[^A-Za-z0-9\_\-\s]/', $form_state['input']['api_ms_name'])) {
    form_set_error('api_ms_name', t('API Name must only contains letters, digits, spaces, dashes, and underscores.'));
  }
}


/**
 * Helper function to Change API Name
 *
 * @param array $form
 * @param array $form_state
 */
function api_management_change_api_name_form_submit($form, $form_state) {

  // Check if Cancel button is clicked
  if (strpos($form_state['clicked_button']['#id'], '-cancel--')) {
    return;
  }

  // Get Taxonomy Terms...
  $query = new EntityFieldQuery;
  $result = $query
    ->entityCondition('entity_type', 'taxonomy_term')
    ->propertyCondition('name', $form_state['input']['api_name'])
    ->propertyCondition('vid', VID_API_NAME)
    ->execute();

  // Update API Console
  foreach ($result['taxonomy_term'] as $tid => $info) {
    // dpm($tid . '---'. $form_state['input']['api_name'] . '---'. VID_API_NAME . ' -- '. $form_state['input']['api_ms_name']);

    // Update API Console
    $query = db_select('field_data_field_api_name_console', 'a');
    $query->fields('a', array('entity_id'));
    $query->condition('a.field_api_name_console_tid', $tid);
    $result = $query->execute();
    $item = $result->fetch();
    if ($item->entity_id) {
      $xnode = node_load($item->entity_id);
      $xnode->title = $form_state['input']['api_ms_name'];
      node_save($xnode);
    }
  }

  // Update API Product
  $xnode = node_load($form_state['input']['api_nid']);
  $xnode->field_api_display_name[LANGUAGE_NONE][0]['value'] = $form_state['input']['api_ms_name'];
  node_save($xnode);

  // Update Forum API NAME Term
  db_update('taxonomy_term_data')
    ->fields(array('name' => $form_state['input']['api_ms_name']))
    ->condition('name', $form_state['input']['api_name'])
    ->condition('vid', VID_API_NAME)
    ->execute();

  // Update Forum Taxonomy Term
  db_update('taxonomy_term_data')
    ->fields(array('name' => $form_state['input']['api_ms_name']))
    ->condition('name', $form_state['input']['api_name'])
    ->condition('vid', VID_FORUMS)
    ->execute();

  // Update Name of Role
  db_update('role')
    ->fields(array('name' => $form_state['input']['api_ms_name']))
    ->condition('name', $form_state['input']['api_name'])
    ->execute();

  watchdog('api_management', 'Changed API name from %api to %api2', array('%api' => $form_state['input']['api_name'], '%api2' => $form_state['input']['api_ms_name']), WATCHDOG_INFO);
}

//########## //########## //########## //########## //##########

/**
 * A modal login callback.
 */
function _api_management_ajax_delete_api($js = NULL, $name, $id) {
  // Fall back if $js is not set.
  if (!$js) {
    return drupal_get_form('api_management_delete_api_form');
  }

  ctools_include('modal');
  ctools_include('ajax');
  $form_state = array(
    'title' => t('Delete API - ' . $name),
    'ajax' => TRUE,
  );

  $output = ctools_modal_form_wrapper('api_management_delete_api_form', $form_state);

  if (!empty($form_state['executed'])) {
    // We'll just overwrite the form output if it was successful.
    $output = array();
    $output[] = ctools_modal_command_dismiss();

    if (!strpos($form_state['clicked_button']['#id'], '-cancel--')) {
      $output[] = ajax_command_remove("a.api-remove-$id");
      $output[] = ajax_command_replace("tr.api-row-".$id. " .views-field-status", '<td class="views-field views-field-status red"> DELETED  </td>');
    }
  }
  print ajax_render($output);
  exit;
}

/**
 * Form constructor for deleting an API.  Built via a menu callback to drupal_get_form().
 *
 * Incoming URL: admin/api/%ctools_js/api_management_delete_api_form/%
 *
 * @return array
 */
function api_management_delete_api_form() {
  $form = array();

  // Proceed if API Product does not have users
  if (api_management_verify_developer_count(arg(4))) {
    $form['api_warning'] = array(
      '#markup' => t('Warning, you can not alter the status of an API Product that contains developers.'),
    );
    return $form;
  }

  $form['api_warning'] = array(
    '#markup' => t('Warning, you are about to permanently: <br><br>
                  - delete all Blogs, API Resource Docs, API Console, and Forums associated to "' . arg(4) . '"<br><br>
                  - delete the role "'. arg(4) . '" <br><br>
                  - disassociate all users associated to the API Product "' . arg(4) . '" <br><br>'
                  ),
  );

  $form['api_name'] = array(
    '#type' => 'hidden',
    '#value' => arg(4),
  );

  $form['api_id'] = array(
    '#type' => 'hidden',
    '#value' => arg(5),
  );

  $form['api_nid'] = array(
    '#type' => 'hidden',
    '#value' => arg(6),
  );

  $form['cancel'] = array(
    '#prefix' => '<br>',
    '#type' => 'submit',
    '#value' => 'Cancel' ,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Delete API' ,
  );

  $form['#submit'][0] = 'api_management_delete_api_form_submit';

  return $form;
}


/**
 * Delete API...
 */
function api_management_delete_api_form_submit($form, $form_state) {
  // Check if Cancel button is clicked
  if (strpos($form_state['clicked_button']['#id'], '-cancel--')) {
    return;
  }

  // Delete User Role
  user_role_delete($form_state['input']['api_name']);

  // Delete API Product
  node_delete($form_state['input']['api_nid']);

  // Check if terms Exist..
  $vids = array(VID_API_NAME, VID_FORUMS);

  foreach($vids as $key => $vid) {
    $query = new EntityFieldQuery;
    $result = $query
      ->entityCondition('entity_type', 'taxonomy_term')
      ->propertyCondition('name', $form_state['input']['api_name'])
      ->propertyCondition('vid', $vid)
      ->execute();

    if (isset($result['taxonomy_term'])) {
      $tid = key($result['taxonomy_term']);

      // Delete Blogs, API Console, API Resource Docs
      if ($vid == VID_API_NAME) {
        // Delete API Console
        $query = db_select('taxonomy_term_data', 't');
        $query->leftJoin('field_data_field_api_name_console', 'a', 't.tid = a.field_api_name_console_tid');
        $query->fields('a', array('entity_id'));
        $query->condition('t.vid', VID_API_NAME);

        $result = $query->execute();
        $items = $result->fetch();
        if ($items->entity_id) {
          // Delete API Product
          node_delete($items->entity_id);
        }

        // Delete API Resource Docs
        $query2 = db_select('taxonomy_term_data', 't');
        $query2->leftJoin('field_data_field_api_name', 'a', 't.tid = a.field_api_name_tid');
        $query2->fields('a', array('entity_id'));
        $query2->condition('t.vid', VID_API_NAME);
        $query2->condition('a.field_api_name_tid', $tid);

        $result2 = $query2->execute();

        while ($item = $result2->fetch()) {
          // Delete API Product
          node_delete($item->entity_id);
        }
      }

      // Delete Forums
      if ($vid == VID_FORUMS) {
        $query2 = db_select('taxonomy_term_data', 't');
        $query2->leftJoin('forum', 'f', 't.tid = f.tid');
        $query2->fields('f', array('nid'));
        $query2->condition('t.vid', VID_FORUMS);
        $query2->condition('f.tid', $tid);

        $result2 = $query2->execute();

        while ($item = $result2->fetch()) {
          // Delete API Product
          node_delete($item->nid);
        }
      }

      // Delete taxonomy terms...
      taxonomy_term_delete($tid);
    }
  }

  watchdog('api_management', 'Deleted API %api', array('%api' => $form_state['input']['api_name']), WATCHDOG_INFO);
}

//########## //########## //########## //########## //##########

/**
 * A modal login callback.
 */
function _api_management_ajax_create_api($js = NULL, $name, $id) {
  // Fall back if $js is not set.
  if (!$js) {
    return drupal_get_form('api_management_create_api_form');
  }

  ctools_include('modal');
  ctools_include('ajax');
  $form_state = array(
    'title' => t('Create API - ' . $name),
    'ajax' => TRUE,
  );

  $output = ctools_modal_form_wrapper('api_management_create_api_form', $form_state);

  if (!empty($form_state['executed'])) {
    // We'll just overwrite the form output if it was successful.
    $output = array();
    $output[] = ctools_modal_command_dismiss();

    if (!strpos($form_state['clicked_button']['#id'], '-cancel--')) {
      $output[] = ajax_command_remove("a.api-remove-$id");
      $output[] = ajax_command_replace("tr.api-row-".$id. " .views-field-status", '<td class="views-field views-field-status red"> ' . $form_state['input']['api_type'] . '  </td>');
    }
  }
  print ajax_render($output);
  exit;
}

/**
 * Form constructor for converting an API to Internal status.  Built via a menu callback to drupal_get_form().
 *
 * Incoming URL: admin/api/%ctools_js/api_management_create_api_form/%
 *
 * @return array
 */
function api_management_create_api_form() {
  $form = array();

  $form['api_warning'] = array(
    '#markup' => t('Create API Product for ' . arg(4) . ':'),
  );

  $form['api_name'] = array(
    '#type' => 'hidden',
    '#value' => arg(4),
  );

  $form['api_id'] = array(
    '#type' => 'hidden',
    '#value' => arg(5),
  );

  $form['api_machine_name'] = array(
    '#type' => 'hidden',
    '#value' => arg(6),
  );

   $form['api_type'] = array(
    '#prefix' => '<br><br>',
    '#type' => 'select',
    '#title' => t('Choose API Type'),
    '#options' => array(
      'public' => t('Public'),
      'private' => t('Private'),
      'internal' => t('Internal'),
    ),
    '#default_value' => 'internal',
  );

  $form['create_api_console'] = array(
    '#type' => 'checkbox',
    '#title' => t('Create API Console placeholder'),
    '#default_value' => TRUE,
  );

  $form['create_resource_doc'] = array(
    '#type' => 'checkbox',
    '#title' => t('Create API Resource Documentation placeholder'),
    '#default_value' => TRUE,
  );

  $form['cancel'] = array(
    '#prefix' => '<br>',
    '#type' => 'submit',
    '#value' => 'Cancel' ,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Create API' ,
  );

  $form['#submit'][0] = 'api_management_create_api_form_submit';

  return $form;
}


/**
 * Internalize API Info...
 */
function api_management_create_api_form_submit($form, $form_state) {

  // Check if Cancel button is clicked
  if (strpos($form_state['clicked_button']['#id'], '-cancel--')) {
    return;
  }

  // Create API Product
  $xnode = new StdClass();
  $xnode->type = 'api_product';
  $xnode->language = LANGUAGE_NONE;
  node_object_prepare($xnode);
  $xnode->title = $form_state['input']['api_machine_name'];
  $xnode->field_api_display_name[LANGUAGE_NONE][0]['value'] = $form_state['input']['api_name'];
  node_save($xnode);

  // Capture NID
  $form_state['input']['api_nid'] = $xnode->nid;

  // Check if terms Exist..
  $vids = array(VID_API_NAME, VID_FORUMS);

  foreach($vids as $key => $vid) {
    $query = new EntityFieldQuery;
    $result = $query
      ->entityCondition('entity_type', 'taxonomy_term')
      ->propertyCondition('name', $form_state['input']['api_name'])
      ->propertyCondition('vid', $vid)
      ->execute();

    $tid = key($result['taxonomy_term']);
    if ($tid) {
      // has tid.
    }
    else {
      // Add new taxonomy term
      $tid = _api_management_create_taxonomy_term($form_state['input']['api_name'], $vid);
    }

    if ($vid == VID_API_NAME) {
      $tid_api = $tid;
    }
  }

  // Create API Console
  if ($form_state['input']['create_api_console']) {
    // Create API Console
    $xnode = new StdClass();
    $xnode->type = 'api_console';
    $xnode->language = LANGUAGE_NONE;
    node_object_prepare($xnode);
    $xnode->title = $form_state['input']['api_name'];
    $xnode->field_api_name_console[LANGUAGE_NONE][0]['tid'] = $tid_api;
    $xnode->field_api_console_embed_code[LANGUAGE_NONE][0]['value'] = '<iframe height="600" scrolling="no" src="https://developer.swisscom.ch" width="100%"></iframe>';
    $xnode->field_api_console_embed_code[LANGUAGE_NONE][0]['format'] = 'full_html';
    node_save($xnode);
  }

  // Create API Resource Product Doc
  if ($form_state['input']['create_api_console']) {
    $xnode = new StdClass();
    $xnode->type = 'api_resource_doc';
    $xnode->language = LANGUAGE_NONE;
    node_object_prepare($xnode);
    $xnode->title = 'GET ' . $form_state['input']['api_name'];
    $xnode->body[LANGUAGE_NONE][0]['value'] = 'A GET method that requires a template parameter, a query parameter and a header.';
    $xnode->body[LANGUAGE_NONE][0]['format'] = 'plain_text';
    $xnode->field_api_method_name[LANGUAGE_NONE][0]['value'] = 'GET';
    $xnode->field_api_name[LANGUAGE_NONE][0]['tid'] = $tid_api;
    $xnode->field_authentication_needed[LANGUAGE_NONE][0]['value'] = 1;
    $xnode->field_parameters[LANGUAGE_NONE][0]['value'] = '[{"parameter":"param1","required":"true","type":"xsd:string","style":"query","default":"99999","description":"Parameter description.","weight":"0"},{"parameter":"header1","required":"true","type":"xsd:string","style":"header","default":"99999","description":"Parameter description.","weight":"1"}]';
    $xnode->field_path[LANGUAGE_NONE][0]['value'] = 'http://api.mydomain.com/99/resource99.{format}';
    node_save($xnode);

    $xnode = new StdClass();
    $xnode->type = 'api_resource_doc';
    $xnode->language = LANGUAGE_NONE;
    node_object_prepare($xnode);
    $xnode->title = 'PUT ' . $form_state['input']['api_name'];
    $xnode->body[LANGUAGE_NONE][0]['value'] = 'A PUT method that takes form-encoded parameters in the body of the request.';
    $xnode->body[LANGUAGE_NONE][0]['format'] = 'plain_text';
    $xnode->field_api_method_name[LANGUAGE_NONE][0]['value'] = 'PUT';
    $xnode->field_api_name[LANGUAGE_NONE][0]['tid'] = $tid_api;
    $xnode->field_authentication_needed[LANGUAGE_NONE][0]['value'] = 1;
    $xnode->field_parameters[LANGUAGE_NONE][0]['value'] = '[{"parameter":"param1","required":"true","type":"xsd:string","style":"query","default":"77","description":"Parameter description.","weight":"0"},{"parameter":"header1","required":"true","type":"xsd:string","style":"header","default":"77","description":"Parameter description.","weight":"1"}]';
    $xnode->field_path[LANGUAGE_NONE][0]['value'] = 'http://api.mydomain.com/77/resource77.{format}';
    node_save($xnode);
  }


  if ($form_state['input']['api_type'] == 'public') {
    api_management_public_api_form_submit($form, $form_state);
  }
  elseif ($form_state['input']['api_type'] == 'private') {
    api_management_private_api_form_submit($form, $form_state);
  }
  else {
    api_management_internal_api_form_submit($form, $form_state);
  }

  watchdog('api_management', 'Created API %api', array('%api' => $form_state['input']['api_name']), WATCHDOG_INFO);
}

//########## //########## //########## //########## //##########

/**
 * A modal login callback.
 */
function _api_management_ajax_public_api($js = NULL, $name, $id, $nid) {
  // Fall back if $js is not set.
  if (!$js) {
    return drupal_get_form('api_management_public_api_form');
  }

  ctools_include('modal');
  ctools_include('ajax');

  $form_state = array(
    'title' => t('Pubicize API - ' . $name),
    'ajax' => TRUE,
  );

  $output = ctools_modal_form_wrapper('api_management_public_api_form', $form_state);

  if (!empty($form_state['executed'])) {
    $output = array();
    $output[] = ctools_modal_command_dismiss();

    if (!strpos($form_state['clicked_button']['#id'], '-cancel--')) {
      $output[] = ajax_command_remove("a.api-remove-$id");
      $output[] = ajax_command_replace("tr.api-row-".$id. " .views-field-status", '<td class="views-field views-field-status red"> PUBLIC </td>');
    }
  }
  print ajax_render($output);
  exit;
}

/**
 * Form constructor for converting an API to Public status.  Built via a menu callback to drupal_get_form().
 *
 * Incoming URL: admin/api/%ctools_js/api_management_public_api_form/%
 *
 * @return array
 */
function api_management_public_api_form() {
  $form = array();

  // Proceed if API Product does not have users
  if (api_management_verify_developer_count(arg(4))) {
    $form['api_warning'] = array(
      '#markup' => t('Warning, you can not alter the status of an API Product that contains developers.'),
    );
    return $form;
  }

  $form['api_warning'] = array(
    '#markup' => t('Warning, you are about to:<br><br>
                  - remove the role "'. arg(4) .'" associated with this API Product <br><br>
                  - disassociate all users associated the API Product "' . arg(4) . '" <br><br>
                  - restrict Blogs, API Resource Docs, API Console, and Forums to public access "authenticated and anonymous" <br><br>'
                  ),
  );

  $form['api_name'] = array(
    '#type' => 'hidden',
    '#value' => arg(4),
  );

  $form['api_id'] = array(
    '#type' => 'hidden',
    '#value' => arg(5),
  );

  $form['api_nid'] = array(
    '#type' => 'hidden',
    '#value' => arg(6),
  );

  $form['cancel'] = array(
    '#prefix' => '<br>',
    '#type' => 'submit',
    '#value' => 'Cancel' ,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Publicize API' ,
  );

  $form['#submit'][0] = 'api_management_public_api_form_submit';

  return $form;
}


/**
 * Internalize API Info...
 */
function api_management_public_api_form_submit($form, $form_state) {

  // Check if Cancel button is clicked
  if (strpos($form_state['clicked_button']['#id'], '-cancel--')) {
    return;
  }

  // Load API Product node
  $xnode = node_load($form_state['input']['api_nid']);

  // Erase all roles and set the roles of "Administrator" and "Authenticated" to API Product
  unset($xnode->api_roles[LANGUAGE_NONE]);
  $xnode->api_roles[LANGUAGE_NONE][0] = array('value' => API_ADMIN);
  $xnode->api_roles[LANGUAGE_NONE][1] = array('value' => 2);
  node_save($xnode);

  // Delete User Role - this will clear our TAC Role settings
  user_role_delete($form_state['input']['api_name']);

  // Get Taxonomy Terms...
  $vids = array(VID_API_NAME, VID_FORUMS);
  $query = new EntityFieldQuery;
  $result = $query
    ->entityCondition('entity_type', 'taxonomy_term')
    ->propertyCondition('name', $form_state['input']['api_name'])
    ->propertyCondition('vid', $vids, 'IN')
    ->execute();

  // Add terms Anonymous and Authenticated roles to TAC
  foreach ($result['taxonomy_term'] as $tid => $info) {
    api_management_add_terms_in_TAC(1, $form_state['input']['api_name'], $tid);
    api_management_add_terms_in_TAC(2, $form_state['input']['api_name'], $tid);
  }

  watchdog('api_management', 'Made Public - API %api', array('%api' => $form_state['input']['api_name']), WATCHDOG_INFO);
}

//########## //########## //########## //########## //##########

/**
 * A modal login callback.
 */
function _api_management_ajax_internal_api($js = NULL, $name, $id, $nid) {
  // Fall back if $js is not set.
  if (!$js) {
    return drupal_get_form('api_management_internal_api_form');
  }

  ctools_include('modal');
  ctools_include('ajax');
  $form_state = array(
    'title' => t('Internailze API - ' . $name),
    'ajax' => TRUE,
  );

  $output = ctools_modal_form_wrapper('api_management_internal_api_form', $form_state);

  if (!empty($form_state['executed'])) {
    // We'll just overwrite the form output if it was successful.

    $output = array();
    $output[] = ctools_modal_command_dismiss();

    if (!strpos($form_state['clicked_button']['#id'], '-cancel--')) {
      $output[] = ajax_command_remove("a.api-remove-$id");
      $output[] = ajax_command_replace("tr.api-row-".$id. " .views-field-status", '<td class="views-field views-field-status red"> INTERNAL </td>');
    }
  }
  print ajax_render($output);
  exit;
}

/**
 * Form constructor for converting an API to Internal status.  Built via a menu callback to drupal_get_form().
 *
 * Incoming URL: admin/api/%ctools_js/api_management_internal_api_form/%
 *
 * @return array
 */
function api_management_internal_api_form() {
  $form = array();

  // Proceed if API Product does not have users
  if (api_management_verify_developer_count(arg(4))) {
    $form['api_warning'] = array(
      '#markup' => t('Warning, you can not alter the status of an API Product that contains developers.'),
    );
    return $form;
  }

  $form['api_warning'] = array(
    '#markup' => t('Warning, you are about to:<br><br>
                  - remove the role "'. arg(4) .'" associated with this API Product <br><br>
                  - disassociate all users associated the API Product "' . arg(4) . '" <br><br>
                  - restrict Blogs, API Resource Docs, API Console, and Forums to Internal access <br><br>'
                  ),
  );

  $form['api_name'] = array(
    '#type' => 'hidden',
    '#value' => arg(4),
  );

  $form['api_id'] = array(
    '#type' => 'hidden',
    '#value' => arg(5),
  );

  $form['api_nid'] = array(
    '#type' => 'hidden',
    '#value' => arg(6),
  );

  $form['cancel'] = array(
    '#prefix' => '<br>',
    '#type' => 'submit',
    '#value' => 'Cancel' ,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Internalize API' ,
  );

  $form['#submit'][0] = 'api_management_internal_api_form_submit';

  return $form;
}


/**
 * Internalize API Info...
 */
function api_management_internal_api_form_submit($form, $form_state) {

  $if_public = 0;

  // Check if Cancel button is clicked
  if (strpos($form_state['clicked_button']['#id'], '-cancel--')) {
    return;
  }

  // Clear role from API Product node
  $xnode = node_load($form_state['input']['api_nid']);

  // Remove terms from Anonymous and Authenticated Taxonomy Access Control
  foreach ($xnode->api_roles[LANGUAGE_NONE] as $key => $value) {
    if ($value['value'] <= 2) {
      $if_public = 1;
    }
  }

  // Erase all roles and set a single of role "Administrator"
  unset($xnode->api_roles[LANGUAGE_NONE]);
  $xnode->api_roles[LANGUAGE_NONE][0] = array('value' => API_ADMIN);
  node_save($xnode);

  // Delete User Role
  user_role_delete($form_state['input']['api_name']);

  // Delete terms from Anonymous and Authenticated roles
  if ($if_public) {
    api_management_delete_terms_in_TAC(1, $form_state['input']['api_name']);
    api_management_delete_terms_in_TAC(2, $form_state['input']['api_name']);
  }

  watchdog('api_management', 'Made Internal - API %api', array('%api' => $form_state['input']['api_name']), WATCHDOG_INFO);
}


//########## //########## //########## //########## //##########
/**
 * A modal login callback.
 */
function _api_management_ajax_private_api($js = NULL, $name, $id, $nid) {
  // Fall back if $js is not set.
  if (!$js) {
    return drupal_get_form('api_management_private_api_form');
  }

  ctools_include('modal');
  ctools_include('ajax');
  $form_state = array(
    'title' => t('Privatize API - ' . $name),
    'ajax' => TRUE,
  );

  $output = ctools_modal_form_wrapper('api_management_private_api_form', $form_state);

  if (!empty($form_state['executed'])) {
    // We'll just overwrite the form output if it was successful.
    $output = array();
    $output[] = ctools_modal_command_dismiss();

    if (!strpos($form_state['clicked_button']['#id'], '-cancel--')) {
      $output[] = ajax_command_remove("a.api-remove-$id");
      $output[] = ajax_command_replace("tr.api-row-".$id. " .views-field-status", '<td class="views-field views-field-status red"> PRIVATE </td>');
    }

  }
  print ajax_render($output);
  exit;
}


/**
 * Form constructor for converting an API to private status.  Built via a menu callback to drupal_get_form().
 *
 * Incoming URL: admin/api/%ctools_js/api_management_private_api_form/%
 *
 * @return array
 */
function api_management_private_api_form() {
  $form = array();

  $form['api_warning'] = array(
    '#markup' => t('Warning, you are about to:<br><br>
                  - remove all users associated with this API Product "'. arg(4) .'" <br><br>
                  - restrict API Product and Blogs, API Docs, API Console, and Forum to Private access <br><br>'
                  ),
    '#attributes' => array(
      'class' => array(
        'red',
        'biggy'),
      ),
  );

  $form['api_name'] = array(
    '#type' => 'hidden',
    '#value' => arg(4),
  );

  $form['api_id'] = array(
    '#type' => 'hidden',
    '#value' => arg(5),
  );

  $form['api_nid'] = array(
    '#type' => 'hidden',
    '#value' => arg(6),
  );


  $form['cancel'] = array(
    '#prefix' => '<br>',
    '#type' => 'submit',
    '#value' => 'Cancel' ,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Privatize API' ,
  );

  $form['#submit'][0] = 'api_management_private_api_form_submit';

  // dpm($form);

  return $form;
}


/**
 * Remove API Info...
 */
function api_management_private_api_form_submit($form, &$form_state) {

  // Check if Cancel button is clicked
  if (strpos($form_state['clicked_button']['#id'], '-cancel--')) {
    return;
  }

  // Remove Anonymous and Authenticated Roles from API Product and TAC
  $if_public = 0;
  $xnode = node_load($form_state['input']['api_nid']);
  foreach ($xnode->api_roles[LANGUAGE_NONE] as $key => $value) {
    // Remove terms from Anonymous and Authenticated Taxonomy Access Control
    if ($value['value'] <= 2) {
      unset($xnode->api_roles[LANGUAGE_NONE][$key]);
      $if_public = 1;
    }
  }

  // Delete terms from Anonymous and Authenticated roles
  if ($if_public) {
    api_management_delete_terms_in_TAC(1, $form_state['input']['api_name']);
    api_management_delete_terms_in_TAC(2, $form_state['input']['api_name']);
  }

  // Get the role
  $roles = user_roles(TRUE);
  $rid = array_search($form_state['input']['api_name'], $roles);

  // If role does not exist create the role
  if (!$rid) {
    $user_role = new stdClass();
    $user_role->name = $form_state['input']['api_name'];
    // save the role to the Drupal database
    user_role_save($user_role);
  }

  $roles = user_roles(TRUE);
  $rid = array_search($form_state['input']['api_name'], $roles);

  $xnode->api_roles[LANGUAGE_NONE][$rid] = array('value' => $rid);
  node_save($xnode);

  // Check if terms Exist..
  $vids = array(VID_API_NAME, VID_FORUMS);

  foreach($vids as $key => $vid) {
    $query = new EntityFieldQuery;
    $result = $query
      ->entityCondition('entity_type', 'taxonomy_term')
      ->propertyCondition('name', $form_state['input']['api_name'])
      ->propertyCondition('vid', $vid)
      ->execute();

    $tid = key($result['taxonomy_term']);
    if ($tid) {
      // has tid
    }
    else {
      // Add new taxonomy term
      $tid = _api_management_create_taxonomy_term($form_state['input']['api_name'], $vid);
    }

    api_management_add_terms_in_TAC($rid, $form_state['input']['api_name'], $tid);
  }

  watchdog('api_management', 'Make Private - API %api', array('%api' => $form_state['input']['api_name']), WATCHDOG_INFO);
}


function api_management_add_terms_in_TAC($rid, $api_name, $tid) {
  module_load_include('inc', 'taxonomy_access', 'taxonomy_access.admin');
  $taxo_form = drupal_get_form('taxonomy_access_admin_form', $rid);

  $simplified_form1['selected_defaults'] = $taxo_form['selected_defaults'];
  $simplified_form1['selected_terms'] = $taxo_form['selected_terms'];
  $simplified_form2['grants'] = $taxo_form['grants'];
  $simplified_form2['new'] = $taxo_form['new'];

  $simplified_form2['grants']['global00'] = $simplified_form2['grants'][0];
  $values = _api_management_prep_TAC($simplified_form1, $simplified_form2);

  $values['values']['new']['grants']['view'] = 1;
  $values['values']['new']['grants']['list'] = 1;
  $values['values']['new']['item'] = 'term ' . $tid;
  _taxonomy_access_admin_form_add($rid, $values['values']);
}


//########## //########## //########## //########## //##########

/**
 * Verify if API Function has users..
 */
function api_management_verify_developer_count($api_name = null) {
  $user_roles = _api_management_get_api_roles();
  $rid = array_search($api_name, $user_roles);

  // Get list of Developers assigned to this API Product
  $query = db_select('users_roles', 'ur');
  $query->leftJoin('users', 'u', 'u.uid = ur.uid');
  $query->fields('u', array('uid'));
  $query->condition('ur.rid', $rid);
  $result = $query->execute();
  $item = $result->fetchAll();

  if (count($item)) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}



/**
 * Call this function when API Management page is first loaded to initialize
 */
function api_management_reset_display($js, $action) {
  if (!$js) {
    // We don't support degrading this from js because we're not
    // using the server to remember the state of the table.
    return MENU_ACCESS_DENIED;
  }

  ctools_include('modal');
  ctools_include('ajax');
  $form_state = array(
    'title' => t('Login'),
    'ajax' => TRUE,
  );
  $output = ctools_modal_form_wrapper('mymodule_form', $form_state);

  print ajax_render($output);
  exit;
}


/**
 * Post-login processor: should we go to the user account or stay in place?
 */
function api_management_ajax_cancel($js) {
  if (!$js) {
    // we should never be here out of ajax context
    return MENU_NOT_FOUND;
  }

  ctools_include('ajax');
  ctools_add_js('ajax-responder');
  $commands = array();
  $commands[] = ctools_ajax_command_reload();

  print ajax_render($commands);
  exit;
}

//########## //########## //########## //########## //##########

/**
* Create a taxonomy term and return the tid.
*/
function _api_management_create_taxonomy_term($name, $vid, $parent_id = 0) {
  $term = new stdClass();
  $term->name = $name;
  $term->vid = $vid;
  $term->description = 'Questions and discussions about the Swisscom ' . $name . ' API.';
  $term->parent = array($parent_id);
  taxonomy_term_save($term);
  return $term->tid;
}


function api_management_delete_terms_in_TAC($rid, $api_name) {
  module_load_include('inc', 'taxonomy_access', 'taxonomy_access.admin');
  $taxo_form = drupal_get_form('taxonomy_access_admin_form', $rid);

  $simplified_form1['selected_defaults'] = $taxo_form['selected_defaults'];
  $simplified_form1['selected_terms'] = $taxo_form['selected_terms'];
  $simplified_form2['grants'] = $taxo_form['grants'];
  $simplified_form2['new'] = $taxo_form['new'];

  $simplified_form2['grants']['global00'] = $simplified_form2['grants'][0];
  $values = _api_management_prep_TAC($simplified_form1, $simplified_form2);


  $vids = array(VID_API_NAME, VID_FORUMS);
  $query = new EntityFieldQuery;
  $result = $query
    ->entityCondition('entity_type', 'taxonomy_term')
    ->propertyCondition('name', $api_name)
    ->propertyCondition('vid', $vids, 'IN')
    ->execute();

  $got_tids = 0;
  foreach ($result['taxonomy_term'] as $tid => $info) {
    $values['values']['selected_terms'][$tid] = 1;
    $got_tids = 1;
  }

  if ($got_tids) {
    _taxonomy_access_admin_form_delete($rid, $values['values']);
  }
}


function _api_management_prep_TAC($form1, $form2){
  $values = array();
  $exclude = array('#parents', '#array_parents');

  _api_management_get_values1($form1, &$values);

  foreach($form2['grants'] as $parent2 => $values1) {
    if (!in_array($parent2, $exclude)) {
      _api_management_get_values2($values1, &$values, '', 'grants', $parent2);
    }
  }

  foreach($form2['new'] as $parent2 => $values1) {
    if (!in_array($parent2, $exclude)) {
      _api_management_get_values3($values1, &$values, '', 'new', $parent2);
    }
  }
  $values['new']['item'] = 'term REPLACE WITH TID';
  $values['new']['recursive'] = 0;
  $values['new']['add'] = 'Add ';

  $form_state = array();
  $form_state['values'] = $values;
  return ($form_state);
}

function _api_management_get_values3($element, &$values, $parent = null, $parent1, $parent2) {
  foreach (element_children($element) as $key) {
    if (!empty($element[$key])) {
      if (isset($element[$key]['#default_value'])) {
        $values[$parent1][$parent2][$key] = $element[$key]['#default_value'];
      } else {
        // Recurse through all children elements.
        _api_management_get_values3($element[$key], &$values, $key, $parent1, $parent2);
      }
    }
  }
}


function _api_management_get_values2($element, &$values, $parent = null, $parent1, $parent2) {
  foreach (element_children($element) as $key) {
    if (!empty($element[$key])) {
      if (isset($element[$key]['#default_value'])) {
        if ($parent2 == 'global00') {
          $values[$parent1][0][$parent][$key] = $element[$key]['#default_value'];
        }
        else {
          $values[$parent1][$parent2][$parent][$key] = $element[$key]['#default_value'];
        }
      } else {
        // Recurse through all children elements.
        _api_management_get_values2($element[$key], &$values, $key, $parent1, $parent2);
      }
    }
  }
}

function _api_management_get_values1($element, &$values, $parent = null) {
  foreach (element_children($element) as $key) {
    if (isset($element[$key]) && $element[$key]) {
      if (isset($element[$key]['#value'])) {
        $values[$parent][$key] = $element[$key]['#value'];
      } else {
        // Recurse through all children elements.
        _api_management_get_values1($element[$key], &$values, $key);
      }
    }
  }
}
