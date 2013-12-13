<?php

 /**
 * @file
 * API Management module include file.
 *
 * @author
 * Cesar Galindo <cesar@apigee.com>
 *
 * @since 2013-01-Sept
 */

/*
 *  Function to Add/Remove Developers, Blogs, Console, Docs, etc from an API Product
 */
function api_management_edit($api, $status) {
  $module_path = drupal_get_path('module', 'api_management');
  drupal_add_css("$module_path/api_management.css");

  // Breaks Javascript if not loaded
  drupal_add_library('system', 'ui.datepicker');

  drupal_set_title(t("API - " . $api));

  // Include the CTools tools that we need.
  ctools_include('ajax');
  ctools_include('modal');

  // Add CTools' javascript to the page. -- this is needed to make module pop ups work
  ctools_modal_add_js();

  $output = '<a href="/admin/config/api-management" class="pull-right">Back to API Management</a><br>';

  // ################ Process Developers ################
  $output .= '<h3>Developers</h3> <span class="developer_red"></span>';
  $header = array(
    array('data' => 'UID'),
    array('data' => 'Username'),
    array('data' => 'Email'),
    array('data' => 'First Name'),
    array('data' => 'Last Name'),
    array('data' => 'Remove'),
  );

  $rows = array();

  $user_roles = _api_management_get_api_roles();
  $rid = array_search($api , $user_roles);

  // Get list of Developers assigned to this API Product
  $query = db_select('users_roles', 'ur');
  $query->leftJoin('users', 'u', 'u.uid = ur.uid');
  $query->leftJoin('field_data_field_first_name', 'f', 'f.entity_id = ur.uid');
  $query->leftJoin('field_data_field_last_name', 'l', 'l.entity_id = ur.uid');
  $query->fields('u', array('name', 'mail', 'uid'));
  $query->fields('f', array('field_first_name_value'));
  $query->fields('l', array('field_last_name_value'));
  $query->condition('ur.rid', $rid);
  $result = $query->execute();

  while ($item = $result->fetch()) {
    $id = $item->uid;
    $username = $item->name;
    $row = array(
        $id,
        $username,
        $item->mail,
        $item->field_first_name_value,
        $item->field_last_name_value,
        ctools_modal_text_button(t("Remove"), "admin/api/nojs/api_management_remove_user_api_form/$username/$id/$api", t('Remove')),
    );

    $rows[] = array('data' => $row, 'class' => array('api-remove-'.$id));
  }

  if ($status == 'Private') {
    $row = array( array ('data' => ctools_modal_text_button(t("<em>Add New Developer</em>"), "admin/api/nojs/api_management_add_user_api_form/$api", t('<em>Add New Developer</em>')), 'colspan' => 3));
  }
  else {
    $row = array( array ('data' => t('<em>Change API status to Private to add developers.'), 'colspan' => 3));
  }
  $rows[] = $row;

  $output .= theme(
    'table',
    array(
      'header' => $header,
      'rows' => $rows,
      'attributes'=> array(),
      'caption' => '',
      'colgroups' => array(),
      'sticky' => false,
      'empty' => false,
    )
  );

  // Reset rows arrays
  $rows_api_console = array();
  $rows_api_resource_doc = array();
  $rows_blog = array();
  $rows_forum = array();

  // Get NIDS for API Console, Forums, Blogs, API Resource Docs
  $vids = array(VID_API_NAME, VID_FORUMS);

  foreach($vids as $key => $vid) {
    $query = new EntityFieldQuery;
    $result = $query
      ->entityCondition('entity_type', 'taxonomy_term')
      ->propertyCondition('name', $api)
      ->propertyCondition('vid', $vid)
      ->execute();

    if (isset($result['taxonomy_term'])) {
      $tid = key($result['taxonomy_term']);

      // VID for Blogs, API Console, API Resource Docs
      if ($vid == VID_API_NAME) {

        // Process API Console
        $query = db_select('taxonomy_term_data', 't');
        $query->leftJoin('field_data_field_api_name_console', 'a', 't.tid = a.field_api_name_console_tid');
        $query->fields('a', array('entity_id'));
        $query->condition('t.tid', $tid);
        $result = $query->execute();
        $item = $result->fetch();

        if ($item->entity_id) {
          $api_console_nid = $item->entity_id;
          $nid = $item->entity_id;
          $row = array(
              $nid,
              $api,
              ctools_modal_text_button(t("Remove"), "admin/api/nojs/api_management_remove_api_console_form/$api/$nid/$tid", t('Remove')),
          );
          $rows_api_console[] = array('data' => $row, 'class' => array('api-remove-'.$nid));
        }

        $row = array( array ('data' => ctools_modal_text_button(t("<em>Add API Console</em>"), "admin/api/nojs/api_management_add_api_console_form/$api/$tid/$tid", t('<em>Add API Console</em>')), 'colspan' => 3));
        $rows_api_console[] = $row;

        // Process API Resource Docs
        $query2 = db_select('taxonomy_term_data', 't');
        $query2->leftJoin('field_data_field_api_name', 'a', 't.tid = a.field_api_name_tid');
        $query2->leftJoin('node', 'n', 'a.entity_id = n.nid');
        $query2->fields('a', array('entity_id'));
        $query2->fields('n', array('title'));
        $query2->condition('t.vid', VID_API_NAME);
        $query2->condition('a.field_api_name_tid', $tid);

        $result2 = $query2->execute();

        while ($item = $result2->fetch()) {
          $nid = $item->entity_id;
          $title = $item->title;
          $row = array(
              $nid,
              $title,
              ctools_modal_text_button(t("Remove"), "admin/api/nojs/api_management_remove_api_doc_form/$api/$nid/$title", t('Remove')),
          );
          $rows_api_resource_doc[] = array('data' => $row, 'class' => array('api-remove-'.$nid));
        }

        $row = array( array ('data' => ctools_modal_text_button(t("<em>Add API Resource Doc</em>"), "admin/api/nojs/api_management_add_api_doc_form/$api/$tid/$tid", t('<em>Add API Resource Doc</em>')), 'colspan' => 3));
        $rows_api_resource_doc[] = $row;
      }

      // Process API Forums
      if ($vid == VID_FORUMS) {
        // Display Forum link
        $api_path = str_replace(" ", "-", $api);
        $row = array( array ('data' => '<a href="/forums/' . $api_path . '" target="_blank"><b>' . $api . '</b></a>', 'colspan' => 3));
        $rows_forum[] = array('data' => $row);

        // Display Forum posts
        $query2 = db_select('taxonomy_term_data', 't');
        $query2->leftJoin('forum', 'f', 't.tid = f.tid');
        $query2->fields('f', array('nid'));
        $query2->fields('t', array('name'));
        $query2->condition('t.vid', VID_FORUMS);
        $query2->condition('f.tid', $tid);
        $result2 = $query2->execute();
        while ($item = $result2->fetch()) {
          $nid = $item->nid;
          $title = $item->name;
          $row = array(
              $nid,
              $api,
              '<a href="/node/' . $nid .'/edit" target="_blank">Edit</a>',
          );
          $rows_forum[] = array('data' => $row, 'class' => array('api-remove-'.$nid));
        }
      }
    }

  }

  // ################ Process API Console ################
  $output .= '<h3>API Console</h3> <span class="api_console_red"></span>';
  $header = array(
    array('data' => 'NID'),
    array('data' => 'Name'),
    array('data' => 'Remove'),
  );

  $output .= theme(
    'table',
    array(
      'header' => $header,
      'rows' => $rows_api_console,
      'attributes'=> array(),
      'caption' => '',
      'colgroups' => array(),
      'sticky' => false,
      'empty' => false,
    )
  );

  // ################ Process API Resource Docs ################
  $output .= '<h3>API Resource Documentation</h3> <span class="api_doc_red"></span>';
  $output .= theme(
    'table',
    array(
      'header' => $header,
      'rows' => $rows_api_resource_doc,
      'attributes'=> array(),
      'caption' => '',
      'colgroups' => array(),
      'sticky' => false,
      'empty' => false,
    )
  );

  // ################ Process API Forums ################
  $header = array(
    array('data' => 'NID'),
    array('data' => 'Name'),
    array('data' => 'Edit'),
  );
  $output .= '<h3>API Forum</h3> <span class="api_forum_red"></span>';
  $output .= theme(
    'table',
    array(
      'header' => $header,
      'rows' => $rows_forum,
      'attributes'=> array(),
      'caption' => '',
      'colgroups' => array(),
      'sticky' => false,
      'empty' => false,
    )
  );

  $output .= '<br><a href="/admin/config/api-management" class="pull-right">Back to API Management</a>';

  return $output;
}


//########## //########## //########## //########## //##########
function _api_management_update_developer_count($api_name = null) {

  // Get list of API Products stored in Drupal DB
  $query = db_select('node', 'n');
  $query->leftJoin('field_data_field_api_display_name', 'd', 'n.nid = d.entity_id');
  $query->fields('n', array('nid'));
  $query->fields('d', array('field_api_display_name_value'));
  $query->condition('n.type', 'api_product');
  // Restrict to One API Product...
  if ($api_name) {
    $query->condition('d.field_api_display_name_value', $api_name);
  }
  $result = $query->execute();


  $user_roles = _api_management_get_api_roles();

  while ($item = $result->fetch()) {
    $rid = array_search($item->field_api_display_name_value, $user_roles);

    // Get list of Developers assigned to this API Product
    $query2 = db_select('users_roles', 'ur');
    $query2->leftJoin('users', 'u', 'u.uid = ur.uid');
    $query2->fields('u', array('name', 'mail', 'uid'));
    $query2->condition('ur.rid', $rid);
    $result2 = $query2->execute();
    $item2 = $result2->fetchAll();

    $xnode = node_load($item->nid);
    if (count($item2)) {
      $xnode->field_api_developer_count[LANGUAGE_NONE][0]['value'] = count($item2);
    }
    else {
      $xnode->field_api_developer_count = array();
    }

    node_save($xnode);
  }

  if ($api_name == null) {
    drupal_goto('/admin/config/api-management');
  }

}


//########## //########## //########## //########## //##########

/**
 * A modal login callback to add API Docs
 */
function _api_management_ajax_add_api_doc($js = NULL, $api, $nid, $tid) {
  // Fall back if $js is not set.
  if (!$js) {
    return drupal_get_form('api_management_add_api_doc_form');
  }

  ctools_include('modal');
  ctools_include('ajax');
  $form_state = array(
    'title' => t('Add API Resource Doc'),
    'ajax' => TRUE,
  );

  $output = ctools_modal_form_wrapper('api_management_add_api_doc_form', $form_state);
  if (!empty($form_state['executed'])) {
    // We'll just overwrite the form output if it was successful.
    $output = array();
    $output[] = ctools_modal_command_dismiss();

    if (!strpos($form_state['clicked_button']['#id'], '-cancel--')) {
      $output[] = ajax_command_replace("span.api_doc_red",  '<span class="api_doc_red red">' . t('Added API Console') . ' ' . $form_state['input']['api_name'] . '</span>');
    }
  }
  print ajax_render($output);
  exit;
}

/**
 * Form constructor for adding an API.  Built via a menu callback to drupal_get_form().
 *
 * Incoming URL: admin/api/%ctools_js/api_management_add_api_doc_form/%
 *
 * @return array
 */
function api_management_add_api_doc_form() {
  $form = array();

  $form['api_warning'] = array(
    '#markup' => t('Enter name of API Resource Doc to assign to API "' . arg(4) . '. <br> Warning, assigning an existing API Resource Doc will override its association to another API.'),
  );

  $form['api_name'] = array(
    '#type' => 'hidden',
    '#value' => arg(4),
  );

  $form['api_nid'] = array(
    '#type' => 'hidden',
    '#value' => arg(5),
  );

  $form['api_tid'] = array(
    '#type' => 'hidden',
    '#value' => arg(6),
  );

  $form['api_doc'] = array(
    '#title' => t('API Resource Doc'),
    '#type' => 'textfield',
    '#autocomplete_path' => 'admin/api/get_api_doc',
  );

  $form['cancel'] = array(
    '#prefix' => '<br><br>',
    '#type' => 'submit',
    '#value' => 'Cancel' ,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Add' ,
  );

  $form['#submit'][0] = 'api_management_add_api_doc_form_submit';

  return $form;
}

/**
 * Autocomplete callback function to retrieve API Resource Docs
 *
 * @param string $string
 */
function _api_management_get_api_doc($string) {
  $matches = array();
  $query = db_select('node', 'n');

  // Select rows that match the string
  $return = $query
    ->fields('n', array('title'))
    ->condition('n.type', 'api_resource_doc')
    ->condition('n.title', '%' . db_like($string) . '%', 'LIKE')
    ->range(0, 10)
    ->execute();

  // add matches to $matches
  foreach ($return as $row) {
    $matches[$row->title] = check_plain($row->title);
  }

  // return for JS
  drupal_json_output($matches);
}

/**
 * Helper handler to add API Resource Doc to API Product
 *
 * @param array $form
 * @param array $form_state
 */
function api_management_add_api_doc_form_submit($form, $form_state) {
  // Check if Cancel button is clicked
  if (strpos($form_state['clicked_button']['#id'], '-cancel--')) {
    return;
  }

  $module_path = drupal_get_path('module', 'api_management');
  drupal_add_css("$module_path/api_management.css");

  if ($form_state['input']['api_tid']) {
    $query = db_select('node', 'n');
    $query->fields('n', array('nid'));
    $query->condition('n.title', $form_state['input']['api_doc']);
    $query->condition('n.type',  'api_resource_doc');
    $result = $query->execute();
    $item = $result->fetch();

    $xnode = node_load($item->nid);
    $xnode->field_api_name[LANGUAGE_NONE][0]['tid'] = $form_state['input']['api_tid'];
    // dpm($xnode);
    node_save($xnode);
  }

  watchdog('api_management', 'Added API Resource Doc %title to API %api', array('%title' => $form_state['input']['api_title'], '%api' => $form_state['input']['api_name']), WATCHDOG_INFO);
}

//########## //########## //########## //########## //##########

/**
 * A modal login callback.
 */
function _api_management_ajax_remove_api_doc($js = NULL, $api, $nid, $title) {
  // Fall back if $js is not set.
  if (!$js) {
    return drupal_get_form('api_management_remove_api_doc_form');
  }

  ctools_include('modal');
  ctools_include('ajax');
  $form_state = array(
    'title' => t('Remove API Resource Doc'),
    'ajax' => TRUE,
  );

  $output = ctools_modal_form_wrapper('api_management_remove_api_doc_form', $form_state);

  if (!empty($form_state['executed'])) {
    // We'll just overwrite the form output if it was successful.
    $output = array();
    $output[] = ctools_modal_command_dismiss();

    if (!strpos($form_state['clicked_button']['#id'], '-cancel--')) {
      $output[] = ajax_command_remove("tr.api-remove-$nid");
      $output[] = ajax_command_replace("span.api_doc_red",  '<span class="api_doc_red red">' . t('Removed API Resource Doc') . ' ' . $title . '</span>');
    }
  }
  print ajax_render($output);
  exit;
}

/**
 * Form constructor for deleting an API.  Built via a menu callback to drupal_get_form().
 *
 * Incoming URL: admin/api/%ctools_js/api_management_remove_api_doc_form/%
 *
 * @return array
 */
function api_management_remove_api_doc_form() {
  $form = array();

  $form['api_warning'] = array(
    '#markup' => t('Warning, you are about to remove API Resource Doc "' . arg(6) . '" from API ' . arg(4) . '.'),
  );

  $form['api_name'] = array(
    '#type' => 'hidden',
    '#value' => arg(4),
  );

  $form['api_nid'] = array(
    '#type' => 'hidden',
    '#value' => arg(5),
  );

  $form['api_title'] = array(
    '#type' => 'hidden',
    '#value' => arg(6),
  );

  $form['cancel'] = array(
    '#prefix' => '<br><br>',
    '#type' => 'submit',
    '#value' => 'Cancel' ,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Remove' ,
  );

  $form['#submit'][0] = 'api_management_remove_api_doc_form_submit';

  return $form;
}


/**
 * Remove User API...
 */
function api_management_remove_api_doc_form_submit($form, $form_state) {
  // Check if Cancel button is clicked
  if (strpos($form_state['clicked_button']['#id'], '-cancel--')) {
    return;
  }

  $module_path = drupal_get_path('module', 'api_management');
  drupal_add_css("$module_path/api_management.css");

  // Delete the Taxonomy from Resource Doc
  $xnode = node_load($form_state['input']['api_nid']);
  $xnode->field_api_name = array();
  node_save($xnode);

  watchdog('api_management', 'Removed API Resource Doc %title from API %api', array('%title' => $form_state['input']['api_title'], '%api' => $form_state['input']['api_name']), WATCHDOG_INFO);
}

//########## //########## //########## //########## //##########

/**
 * A modal login callback.
 */
function _api_management_ajax_add_api_console($js = NULL, $api, $nid, $tid) {
  // Fall back if $js is not set.
  if (!$js) {
    return drupal_get_form('api_management_add_api_console_form');
  }

  ctools_include('modal');
  ctools_include('ajax');
  $form_state = array(
    'title' => t('Add API Console'),
    'ajax' => TRUE,
  );

  $output = ctools_modal_form_wrapper('api_management_add_api_console_form', $form_state);

  if (!empty($form_state['executed'])) {
    // We'll just overwrite the form output if it was successful.

    $output = array();
    $output[] = ctools_modal_command_dismiss();

    if (!strpos($form_state['clicked_button']['#id'], '-cancel--')) {
      $output[] = ajax_command_replace("span.api_console_red",  '<span class="api_console_red red">' . t('Added API Console') . ' ' . $form_state['input']['api_name'] . '</span>');
    }
  }
  print ajax_render($output);
  exit;
}

/**
 * Form constructor for deleting an API.  Built via a menu callback to drupal_get_form().
 *
 * Incoming URL: admin/api/%ctools_js/api_management_add_api_console_form/%
 *
 * @return array
 */
function api_management_add_api_console_form() {
  $form = array();

  $form['api_warning'] = array(
    '#markup' => t('Enter name of API Console assign to API "' . arg(4) . '. <br> Warning, assigning an existing API Console will override its association to another API.'),
  );

  $form['api_name'] = array(
    '#type' => 'hidden',
    '#value' => arg(4),
  );

  $form['api_nid'] = array(
    '#type' => 'hidden',
    '#value' => arg(5),
  );

  $form['api_tid'] = array(
    '#type' => 'hidden',
    '#value' => arg(6),
  );

  $form['api_console'] = array(
    '#title' => t('API Console'),
    '#type' => 'textfield',
    '#autocomplete_path' => 'admin/api/get_api_console',
  );

  $form['cancel'] = array(
    '#prefix' => '<br><br>',
    '#type' => 'submit',
    '#value' => 'Cancel' ,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Add' ,
  );

  $form['#submit'][0] = 'api_management_add_api_console_form_submit';

  return $form;
}

function _api_management_get_api_console($string) {
  $matches = array();
  $query = db_select('node', 'n');

  // Select rows that match the string
  $return = $query
    ->fields('n', array('title'))
    ->condition('n.type', 'api_console')
    ->condition('n.title', '%' . db_like($string) . '%', 'LIKE')
    ->range(0, 10)
    ->execute();

  // add matches to $matches
  foreach ($return as $row) {
    $matches[$row->title] = check_plain($row->title);
  }

  // return for JS
  drupal_json_output($matches);
}

/**
 * Remove User API...
 */
function api_management_add_api_console_form_submit($form, $form_state) {
  // Check if Cancel button is clicked
  if (strpos($form_state['clicked_button']['#id'], '-cancel--')) {
    return;
  }

  $module_path = drupal_get_path('module', 'api_management');
  drupal_add_css("$module_path/api_management.css");

  if ($form_state['input']['api_tid']) {
    $query = db_select('node', 'n');
    $query->fields('n', array('nid'));
    $query->condition('n.title', $form_state['input']['api_console']);
    $query->condition('n.type',  'api_console');
    $result = $query->execute();
    $item = $result->fetch();

    $xnode = node_load($item->nid);
    $xnode->field_api_name_console[LANGUAGE_NONE][0]['tid'] = $form_state['input']['api_tid'];
    node_save($xnode);
  }

  watchdog('api_management', 'Added API Console %api to API %api', array('%api' => $form_state['input']['api_name']), WATCHDOG_INFO);
}

//########## //########## //########## //########## //##########

/**
 * A modal login callback.
 */
function _api_management_ajax_remove_api_console($js = NULL, $api, $nid, $nid) {
  // Fall back if $js is not set.
  if (!$js) {
    return drupal_get_form('api_management_remove_api_console_form');
  }

  ctools_include('modal');
  ctools_include('ajax');
  $form_state = array(
    'title' => t('Remove API Console'),
    'ajax' => TRUE,
  );

  $output = ctools_modal_form_wrapper('api_management_remove_api_console_form', $form_state);

  if (!empty($form_state['executed'])) {
    // We'll just overwrite the form output if it was successful.
    $output = array();
    $output[] = ctools_modal_command_dismiss();

    if (!strpos($form_state['clicked_button']['#id'], '-cancel--')) {
      $output[] = ajax_command_remove("tr.api-remove-$nid");
      $output[] = ajax_command_replace("span.api_console_red",  '<span class="api_console_red red">' . t('Removed API Console') . ' ' . $api . '</span>');
    }
  }
  print ajax_render($output);
  exit;
}

/**
 * Form constructor for deleting an API.  Built via a menu callback to drupal_get_form().
 *
 * Incoming URL: admin/api/%ctools_js/api_management_remove_api_console_form/%
 *
 * @return array
 */
function api_management_remove_api_console_form() {
  $form = array();

  $form['api_warning'] = array(
    '#markup' => t('Warning, you are about to remove API Console "' . arg(4) . '" from API ' . arg(4) . '.'),
  );

  $form['api_name'] = array(
    '#type' => 'hidden',
    '#value' => arg(4),
  );

  $form['api_nid'] = array(
    '#type' => 'hidden',
    '#value' => arg(5),
  );

  $form['cancel'] = array(
    '#prefix' => '<br><br>',
    '#type' => 'submit',
    '#value' => 'Cancel' ,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Remove' ,
  );

  $form['#submit'][0] = 'api_management_remove_api_console_form_submit';

  return $form;
}


/**
 * Remove User API...
 */
function api_management_remove_api_console_form_submit($form, $form_state) {
  // Check if Cancel button is clicked
  if (strpos($form_state['clicked_button']['#id'], '-cancel--')) {
    return;
  }

  $module_path = drupal_get_path('module', 'api_management');
  drupal_add_css("$module_path/api_management.css");

  // Delete the role from a User:
  $xnode = node_load($form_state['input']['api_nid']);
  $xnode->field_api_name_console = array();
  node_save($xnode);

  watchdog('api_management', 'Removed API Console %api from API %api', array('%api' => $form_state['input']['api_name']), WATCHDOG_INFO);
}

//########## //########## //########## //########## //##########


/**
 * A modal login callback.
 */
function _api_management_ajax_add_user_api($js = NULL, $api) {
  // Fall back if $js is not set.
  if (!$js) {
    return drupal_get_form('api_management_add_user_api_form');
  }

  ctools_include('modal');
  ctools_include('ajax');
  $form_state = array(
    'title' => t('Add Developer'),
    'ajax' => TRUE,
  );

  $output = ctools_modal_form_wrapper('api_management_add_user_api_form', $form_state);

  if (!empty($form_state['executed'])) {
    // We'll just overwrite the form output if it was successful.

    $output = array();
    $output[] = ctools_modal_command_dismiss();

    if (!strpos($form_state['clicked_button']['#id'], '-cancel--')) {
      $output[] = ajax_command_replace("span.developer_red",  '<span class="developer_red red">' . t('Added developer') . ' ' . $form_state['input']['user_email'] . '</span>');
    }
  }
  print ajax_render($output);
  exit;
}

/**
 * Form constructor for deleting an API.  Built via a menu callback to drupal_get_form().
 *
 * Incoming URL: admin/api/%ctools_js/api_management_add_user_api_form/%
 *
 * @return array
 */
function api_management_add_user_api_form() {
  $form = array();

  $form['api_warning'] = array(
    '#markup' => t('Enter developer email to assign to API "' . arg(4) . '.'),
  );

  $form['api_name'] = array(
    '#type' => 'hidden',
    '#value' => arg(4),
  );


  $form['user_email'] = array(
    '#title' => t('User Email'),
    '#type' => 'textfield',
    '#autocomplete_path' => 'admin/api/getuser',
  );


  $form['cancel'] = array(
    '#prefix' => '<br><br>',
    '#type' => 'submit',
    '#value' => 'Cancel' ,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Add' ,
  );

  $form['#submit'][0] = 'api_management_add_user_api_form_submit';

  return $form;
}

function _api_management_get_user($string) {
  $matches = array();
  $query = db_select('users', 'u');

  // Select rows that match the string
  $return = $query
    ->fields('u', array('mail'))
    ->condition('u.mail', '%' . db_like($string) . '%', 'LIKE')
    ->range(0, 10)
    ->execute();

  // add matches to $matches
  foreach ($return as $row) {
    $matches[$row->mail] = check_plain($row->mail);
  }

  // return for JS
  drupal_json_output($matches);
}

/**
 * Remove User API...
 */
function api_management_add_user_api_form_submit($form, $form_state) {
  // Check if Cancel button is clicked
  if (strpos($form_state['clicked_button']['#id'], '-cancel--')) {
    return;
  }

  $module_path = drupal_get_path('module', 'api_management');
  drupal_add_css("$module_path/api_management.css");

  $add_user = user_load_by_mail($form_state['input']['user_email']);

  // Assigning the role to a user:
  $role = $form_state['input']['api_name'];

  $key = array_search($role, $add_user->roles);
  if ($key == FALSE) {
    $roles = user_roles(TRUE);
    $rid = array_search($role, $roles);
    if ($rid != FALSE) {
      $new_role[$rid] = $role;
      $new_role = $add_user->roles + $new_role;
      user_save($add_user, array('roles' => $new_role));
    }
  }

  _api_management_update_developer_count($form_state['input']['api_name']);

  watchdog('api_management', 'Added developer %devel to API %api', array('%devel' => $form_state['input']['user_email'], '%api' => $form_state['input']['api_name']), WATCHDOG_INFO);
}

//########## //########## //########## //########## //##########

/**
 * A modal login callback.
 */
function _api_management_ajax_remove_user_api($js = NULL, $username, $uid, $api) {
  // Fall back if $js is not set.
  if (!$js) {
    return drupal_get_form('api_management_remove_user_api_form');
  }

  ctools_include('modal');
  ctools_include('ajax');
  $form_state = array(
    'title' => t('Remove Developer'),
    'ajax' => TRUE,
  );

  $output = ctools_modal_form_wrapper('api_management_remove_user_api_form', $form_state);

  if (!empty($form_state['executed'])) {
    // We'll just overwrite the form output if it was successful.
    $output = array();
    $output[] = ctools_modal_command_dismiss();

    if (!strpos($form_state['clicked_button']['#id'], '-cancel--')) {
      $output[] = ajax_command_remove("tr.api-remove-$uid");
      $output[] = ajax_command_replace("span.developer_red",  '<span class="developer_red red">' . t('Removed developer') . ' ' . $username . '</span>');
    }
  }
  print ajax_render($output);
  exit;
}

/**
 * Form constructor for deleting an API.  Built via a menu callback to drupal_get_form().
 *
 * Incoming URL: admin/api/%ctools_js/api_management_remove_user_api_form/%
 *
 * @return array
 */
function api_management_remove_user_api_form() {
  $form = array();

  $form['api_warning'] = array(
    '#markup' => t('Warning, you are about to remove developer "' . arg(4) . '" from API ' . arg(6) . '.'),
  );

  $form['api_username'] = array(
    '#type' => 'hidden',
    '#value' => arg(4),
  );

  $form['api_uid'] = array(
    '#type' => 'hidden',
    '#value' => arg(5),
  );

  $form['api_name'] = array(
    '#type' => 'hidden',
    '#value' => arg(6),
  );

  $form['cancel'] = array(
    '#prefix' => '<br><br>',
    '#type' => 'submit',
    '#value' => 'Cancel' ,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Remove' ,
  );

  $form['#submit'][0] = 'api_management_remove_user_api_form_submit';

  return $form;
}


/**
 * Remove User API...
 */
function api_management_remove_user_api_form_submit($form, $form_state) {
  // Check if Cancel button is clicked
  if (strpos($form_state['clicked_button']['#id'], '-cancel--')) {
    return;
  }

  $module_path = drupal_get_path('module', 'api_management');
  drupal_add_css("$module_path/api_management.css");

  // Delete the role from a User:
  $xuser = user_load($form_state['input']['api_uid']);

  $key = array_search($form_state['input']['api_name'], $xuser->roles);
  unset($xuser->roles[$key]);
  $new_roles = $xuser->roles;
  user_save($xuser, array('roles' => $new_roles));

  _api_management_update_developer_count($form_state['input']['api_name']);

  watchdog('api_management', 'Removed developer %devel from API %api', array('%devel' => $form_state['input']['api_username'], '%api' => $form_state['input']['api_name']), WATCHDOG_INFO);
}
