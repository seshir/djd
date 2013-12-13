<?php

/**
 * Callback to list Partner requests that are recieved.
 */
 function api_partner_management($form,&$form_state) {
	$query = db_select('swisscom_partner_registration', 'spr');
	$query->join('users', 'u', 'spr.uid = u.uid'); //JOIN node with users
	//$query->groupBy('u.uid');//GROUP BY user ID

	$query->fields('spr',array('partner_email','partner_api_product','partner_usage_plan','submitted'))//SELECT the fields from node
    	->fields('u',array('name'))//SELECT the fields from user
	    ->orderBy('spr.submitted', 'DESC');//ORDER BY created

    $rows = array();
	$result = $query->execute();
	foreach($result as $record) {
		$apis = "<ul>";
		$prods = unserialize($record->partner_api_product);
		
		foreach($prods as $nid) {
			$prod = node_load($nid);
			if($prod->nid) 
				$apis .= '<li><a href="/node/'.$prod->nid.'/edit">'.$prod->title.'</a></li>';
		}
		$apis .= "</ul>";
		
		$rows[] = array(
			'partner_email' => $record->partner_email,
			'apis' => $apis,
			'partner_usage_plan' => $record->partner_usage_plan,
			'submitted' => format_date($record->submitted,'short'),
		);
	}
	$header = array('partner_email' => t('Partner Email Address'),'apis' => t('APIs requested'),'partner_usage_plan' => t('Purpose'),'submitted' => t('Submitted on'));
	$output = theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('class' => array('partner_management')) ));
	
	$form['partner_list'] = array(
		'#markup' => $output
	);
	return $form;
 }
 
 
 /**
 * Form builder; Configure site settings.
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function swisscom_apipartner_admin_settings() {
  $form = array();

  $form['description']['#markup'] = t('Settings for API Partner Manager.');

  $form['partner_admin_notify'] = array(
    '#type' => 'fieldset',
    '#title' => t('User Alerts'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE
  );
  $partner_admin_notify_opts = array(
    'Custom' => t('Send to a custom email address'),
    'Role' => t('Send to a specific role(s)'),
    'Both' => t('Send to Both a custom email address and a specific role(s)'),
  );
  $form['partner_admin_notify']['partner_admin_notify_type'] = array(
    '#type' => 'radios',
    '#options' => $partner_admin_notify_opts,
    '#title' => t('Send by Role or Custom Email address?'),
    '#default_value' => variable_get('partner_admin_notify_type', 'Custom'),
    '#prefix' => '<div class="user-register-notify-user-notify">',
    '#suffix' => '</div>',
  );
  $form['partner_admin_notify']['partner_admin_notify_mailto'] = array(
    '#type' => 'textarea',
    '#title' => t('Send notifications to this custom email address'),
    '#description' => t('Please specify multiple email ids with comma(,) separated values eg(%mailto,%mailto). If you leave this blank, the site email address, %mailto, will be used.', array('%mailto' => variable_get('site_mail', ini_get('sendmail_from')))),
    '#default_value' => variable_get('partner_admin_notify_mailto', variable_get('site_mail', ini_get('sendmail_from'))),
  );
  $user_roles = user_roles(TRUE);
  $form['partner_admin_notify']['partner_admin_notify_roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Send to a specific role(s)'),
    '#options' => $user_roles,
    '#description' => t('All users with these checked roles will receive a notification email when selected.'),
    '#default_value' => variable_get('partner_admin_notify_roles', array()),
  );
  
  return system_settings_form($form);
}