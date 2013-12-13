<?php

/**
 * @file
 * Admin page callbacks for the swisscom company module.
 */

function swisscom_company_admin_list() {
  $r = entity_load('swisscom_company');
  dpm($r, 'All companies');
}


function swisscom_company_add_company_form($form, &$form_state) {

}


/**
 * Add new company page callback.
 */
function swisscom_company_add() {
  $company = entity_create('swisscom_company', array());
  drupal_set_title(t('Create Company'));
  $output = drupal_get_form('swisscom_company_form',$company);

  return $output;

//  $task_type = example_task_types($type);
//
//  $task = entity_create('example_task', array('type' => $type));
//  drupal_set_title(t('Create @name', array('@name' => entity_label('task_type', $task_type))));
//
//  $output = drupal_get_form('example_task_form', $task);
//
//  return $output;
}


/**
 * Task Form.
 */
function swisscom_company_form($form, &$form_state, $company) {
    $form_state['company'] = $company;
//
//  $form['title'] = array(
//    '#type' => 'textfield',
//    '#required' => TRUE,
//    '#title' => t('Name'),
//    '#default_value' => $task->title,
//  );
  $form['name'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Name'),
    '#default_value' => $company->name,
  );


//
//  $form['description'] = array(
//    '#type' => 'textarea',
//    '#title' => t('Description'),
//    '#default_value' => $task->description,
//  );
//
//  $form['uid'] = array(
//    '#type' => 'value',
//    '#value' => $task->uid,
//  );
//
//  field_attach_form('swisscom_company', $company, $form, $form_state);
//
  $submit = array();
  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions'] = array(
    '#weight' => 100,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save company'),
    '#submit' => $submit + array('swisscom_company_form_submit'),
  );
//
//  // Show Delete button if we edit task.
//  $task_id = entity_id('example_task' ,$task);
//  if (!empty($task_id) && example_task_access('edit', $task)) {
//    $form['actions']['delete'] = array(
//      '#type' => 'submit',
//      '#value' => t('Delete'),
//      '#submit' => array('example_task_form_submit_delete'),
//    );
//  }
//
  $form['#validate'][] = 'swisscom_company_form_validate';

  return $form;
}

function swisscom_company_form_validate($form, &$form_state) {

}

/**
 * Task submit handler.
 */
function swisscom_company_form_submit($form, &$form_state) {
  $company = $form_state['company'];

  entity_form_submit_build_entity('swisscom_company', $company, $form, $form_state);

  entity_save('swisscom_company', $company);

  $company_uri = entity_uri('swisscom_company', $company);

  $form_state['redirect'] = $company_uri['path'];

  drupal_set_message(t('Company %title saved.', array('%title' => entity_label('swisscom_company', $task))));
}

function example_task_form_submit_delete($form, &$form_state) {
  $task = $form_state['task'];
  $task_uri = entity_uri('example_task', $task);
  $form_state['redirect'] = $task_uri['path'] . '/delete';
}

/**
 * Delete confirmation form.
 */
function example_task_delete_form($form, &$form_state, $task) {
  $form_state['task'] = $task;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['task_type_id'] = array('#type' => 'value', '#value' => entity_id('example_task' ,$task));
  $task_uri = entity_uri('example_task', $task);
  return confirm_form($form,
    t('Are you sure you want to delete task %title?', array('%title' => entity_label('example_task', $task))),
    $task_uri['path'],
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Delete form submit handler.
 */
function example_task_delete_form_submit($form, &$form_state) {
  $task = $form_state['task'];
  example_task_delete($task);

  drupal_set_message(t('Task %title deleted.', array('%title' => entity_label('example_task', $task))));

  $form_state['redirect'] = '<front>';
}
