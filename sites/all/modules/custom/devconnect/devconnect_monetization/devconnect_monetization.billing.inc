<?php

use Apigee\Commerce\Types\BillingType;
use Apigee\Commerce\Types\BillingDocumentType;
use Apigee\Commerce\BillingDocument;
use Apigee\Commerce\ReportDefinition;
use Apigee\Util\Debugger;
use Apigee\Commerce\Exceptions\CommerceApiException;
use Apigee\Commerce\Organization;
use Apigee\Commerce\DataStructures\Address;
use Apigee\Commerce\DataStructures\DeveloperBalance;
use Apigee\Commerce\Developer;

function devconnect_monetization_billing() {
  $active_tab = 'tab1';
  drupal_add_library('system', 'ui.datepicker');
  drupal_add_js(drupal_get_path('module', 'devconnect_monetization') . '/js/global.js', 'file');
  $developer_id = _devconnect_monetization_get_developer_id();
  $client = devconnect_monetization_default_api_client();
  $developer = new Developer($client);

  $developer->load($developer_id);

  $form = drupal_get_form('devconnect_monetization_top_up_balance_form');
  $forms = array(
    'top_up_form' => drupal_render($form),
  );

  $organization = new Organization($client);
  $currencies = $organization->listSupportedCurrencies();

  $billing_documents = new BillingDocument($client);
  $billing_months = $billing_documents->listBillingMonths();
  $years = array();
  foreach ($billing_months as $billing_month) {
    if (!in_array($billing_month->getYear(), $years)) {
      $years[$billing_month->getYear()] = $billing_month->getYear();
    }
  }
  rsort($years);
  $months = array();
  foreach ($billing_months as $billing_month) {
    if ($billing_month->getYear() == $years[0]) {
      $months[] = $billing_month->getMonth();
    }
  }

  $accounts = array();
  foreach ($currencies as $currency) {
    $accounts[$currency->name] = "{$currency->name} ({$currency->displayName})";
  }

  $form = drupal_get_form('devconnect_monetization_dowload_prepaid_report_form', $accounts, $years);

  // If developer billing type is PREPAID, then show Prepaid Tab in template
  if ($developer->getBillingType() == BillingType::PREPAID) {
    $balances = $developer->getPrepaidBalance();

    $topped_up_currencies = 0;
    foreach ($balances as $balance) {
      foreach ($currencies as $currency) {
        if ($balance->supportedCurrency->id == $currency->id) {
          $topped_up_currencies++;
          break;
        }
      }
    }
  }
  else {
    $active_tab = 'tab2';
  }

  $revenue_report_form = drupal_get_form('devconnect_monetization_developer_report_form');

  $billing_documents_form = drupal_get_form('devconnect_monetization_billing_document_form');
  $billing_docs_collection = isset($billing_documents_form['#billing_docs']) ? $billing_documents_form['#billing_docs'] : NULL;
  unset($billing_documents_form['#billing_docs']);

  if ($billing_documents_form['#submited']) {
    $active_tab = 'tab3';
  }
  $docs = array();
  foreach ($billing_docs_collection as &$doc) {
    switch ($doc->getType()) {
      case BillingDocumentType::REV_STMT: $type = 'Revenue Share Statement'; break;
      case BillingDocumentType::GPA: $type = 'GPA'; break;
      case BillingDocumentType::NPA: $type = 'NPA'; break;
      case BillingDocumentType::SELF_INVOICE: $type = 'Self Invoice'; break;
      case BillingDocumentType::INVOICE: $type = 'Invoice'; break;
      case BillingDocumentType::NETTING_STMT: $type = 'Netting Statement';  break;
    }
    $products = array();
    foreach ($doc->getProducts() as $product) {
      $products[] = $product->getDisplayName();
    }
    $docs[] = array(
      'type' => $type,
      'ref' => $doc->getDocumentNumber(),
      'prods' => implode(', ', $products),
      'rec_date' => $doc->getBillingDocDate(),
      'id' => $doc->getId(),
    );
  }

  $variables = array(
    'balances' => $developer->getBillingType() == BillingType::PREPAID ? $balances : FALSE,
    'has_balances' => count($balances) > 0,
    'forms' => $forms,
    'currencies' => $currencies,
    'years' => $years,
    'months' => $months,
    'year' => drupal_render($year),
    'account' => drupal_render($account),
    'form' => drupal_render($form),
    'can_top_up_another_currency' => $developer->getBillingType() == BillingType::PREPAID && $topped_up_currencies < count($currencies),
    'revenew_report_form' => drupal_render($revenue_report_form),
    'billing_documents_form' => drupal_render($billing_documents_form),
    'billing_documents' => $docs,
    'active_tab' => $active_tab,
  );
  return theme('devconnect_monetization_billing', $variables);
}

function devconnect_monetization_top_up_balance_form($form, $form_state) {
  if (isset($form_state['values']['top_up_amount'])) {
    $new_form_state = array(
      'values' => array(
        'notes' => '',
        'amount' => $form_state['values']['top_up_amount'],
        'currency_id' => $form_state['values']['currency_id'],
      ),
    );

    devconnect_monetization_top_up_balance_form_submit($form, $new_form_state);
  }

  $form['top_up_amount'] = array(
    '#type' => 'hidden',
  );

  $form['currency_id'] = array(
    '#type' => 'hidden',
  );

  $form['#validate'][] = 'devconnect_monetization_top_up_balance_form_validate';
  $form['#submit'][] = 'devconnect_monetization_top_up_balance_form_submit';

  return $form;
}

function devconnect_monetization_top_up_balance_form_validate(&$form, &$form_state) {
  $top_up_amount = (double)$form_state['values']['top_up_amount'];
  $rate_plancurrency = isset($form_state['values']['currency']) ? $form_state['values']['currency'] : NULL;
  $currency = isset($form_state['values']['currency']) ? $form_state['values']['currency'] : NULL;
}

function devconnect_monetization_top_up_balance_form_submit(&$form, &$form_state) {
  try {
    $currency = strtoupper($form_state['values']['currency_id']);
    if ($GLOBALS['devconnect_monetization_test_user_enabled']) {
      $developer_id = _devconnect_monetization_get_developer_id();
      $user = user_load_by_mail($developer_id);
    }
    else {
      global $user;
    }

    $deposit = commerce_product_new('deposit');

    $deposit->uid = $user->uid;
    $deposit->sku = 'deposit_' . $user->uid . '_' . $form_state['values']['amount'] . '_' . microtime();

    $deposit->commerce_price['und'][0]['amount'] = $form_state['values']['amount'] * 100;
    $deposit->commerce_price['und'][0]['currency_code'] = $currency;

    $deposit->title = 'Deposit ' . commerce_currency_format($form_state['values']['amount'] * 100, $currency);

    commerce_product_save($deposit);

    $deposit_order = ($user->uid) ? commerce_order_new($user->uid, 'checkout_checkout') : commerce_cart_order_new();

    $line_item = commerce_product_line_item_new($deposit, 1, $deposit_order->order_id);

    commerce_line_item_save($line_item);

    $order_wrapper = entity_metadata_wrapper('commerce_order', $deposit_order);

    $order_wrapper->commerce_line_items[] = $line_item;


    $deposit_order->data['type'] = 'commerce_funds_deposit';
    $deposit_order->data['notes'] = $form_state['values']['notes'];

    commerce_order_save($deposit_order);

    drupal_goto('checkout/' . $deposit_order->order_id);

  }
  catch(Exception $e) {
    drupal_set_message('The website encountered an unexpected error. Please try again later. ', 'error');
    watchdog_exception('devconnect_monetization', $e, $e->getMessage(), array(), WATCHDOG_CRITICAL);
  }
}

function devconnect_monetization_download_report($currency_id, $date) {
  $currency_id = rawurldecode($currency_id);
  $date = rawurldecode($date);

  try {
    $developer_id = _devconnect_monetization_get_developer_id();
    $client = devconnect_monetization_default_api_client();

    list($month, $year) = explode('-', $date);

    $org = new Organization($client);
    $response = $org->getPrepaidBalanceReport($month, $year, $developer_id, $currency_id);

    header('Content-Disposition: attachment; filename="Prepaid_Balance-' . $month . $year . '_' . time() . '.csv"');
    header('Content-Type: application/octet-stream;charset=utf-8');
    header('Content-Length: ' . strlen($response));
    header('Connection: close');

    echo $response;
  }
  catch (CommerceApiException $cae) {
    if ( $cae->getCommerceCode() == CommerceApiException::RESOURCE_DOES_NOT_EXIST) {
      drupal_set_message(t('There is no report for account @account in @month-@year', array(
        '@account' => $currency_id,
        '@month' => $month,
        '@year' => $year,
      )), 'error');
      drupal_goto('users/me/monetization/billing');
    }
    else {
      if ($GLOBALS['devconnect_monetization_debug_endpoint_response']) {
        drupal_set_message('DEBUG: ' . $cae->getCommerceMessage(TRUE), 'warning');
      }
      throw $cae;
    }
  }
  catch (Exception $e) {
    drupal_set_message('The website encountered an unexpected error. Please try again later. ', 'error');
    watchdog_exception('devconnect_monetization', $e, $e->getMessage(), array(), WATCHDOG_CRITICAL);
    drupal_goto('users/me/monetization/billing');
  }
}

function devconnect_monetization_dowload_prepaid_report_form($form, $form_state) {
  $accounts = $form_state['build_info']['args'][0];
  $years = array_combine($form_state['build_info']['args'][1], $form_state['build_info']['args'][1]);
  $form = array();
  $form['validation_message'] = array(
    '#markup' => '<div id="donwload_previous_prepaid_statements_error_div" class="alert alert-block alert-error fade in hide"><h4 class="alert-heading">Make sure you have:</h4><p></p></div>',
  );
  $form['account'] = array(
    '#type' => 'select',
    '#title' => t('Select an account'),
    '#options' => array(-1 => t('Select an account')) + $accounts,
  );
  $years = $years !== FALSE ? $years : array();

  $form['year'] = array(
    '#type' => 'select',
    '#title' => t('Select a year'),
    '#options' => array(-1 => t('Select a year')) + $years,
    '#ajax' => array(
      'callback' => 'devconnect_monetization_billing_get_billing_months_by_year',
    ),
  );
  $form['month'] = array(
    '#type' => 'select',
    '#title' => t('Select a month'),
    '#options' => array(-1 => t('Select a month')),
  );
  $form['submit'] = array(
    '#markup' => '<a id="donwload_previous_prepaid_statements_anchor" class="btn">' . t('Download CSV') . '</a>'
  );
  return $form;
}

function devconnect_monetization_billing_get_billing_months_by_year($form, &$form_state) {

  $year = $form_state['values']['year'];
  $client = devconnect_monetization_default_api_client();
  $billing_document = new BillingDocument($client);
  $billing_months = $billing_document->listBillingMonths();
  $months = array();
  foreach ($billing_months as $billing_month) {
    if ($billing_month->getYear() == $year) {
      $months[(int)$billing_month->getMonth()] = $billing_month->getMonthEnum();
    }
  }
  ksort($months);
  $options = '<option value="-1">Select a month</option>';
  foreach ($months as $month) {
    $options .= '<option value="' . $month . '">' . ucwords(strtolower($month)) . '</option>';
  }

  $commands = array();
  $commands[] = ajax_command_replace('#devconnect-monetization-dowload-prepaid-report-form select:last option', $options);
  $commands[] = ajax_command_invoke('#devconnect-monetization-dowload-prepaid-report-form select:last option:first-child', 'attr', array('disabled', true));
  $commands[] = ajax_command_invoke('#devconnect-monetization-dowload-prepaid-report-form select:last option:first-child', 'attr', array('selected', true));
  $commands[] = ajax_command_invoke(NULL, 'fixBillingMonthSelect');
  return array('#type' => 'ajax', '#commands' => $commands);
}

function devconnect_monetization_billing_document_form($form, &$form_state) {
  $developer_id = _devconnect_monetization_get_developer_id();
  $client = devconnect_monetization_default_api_client();

  $billing_document = new BillingDocument($client);
  $billing_months = $billing_document->listBillingMonths();

  $months = array();
  foreach ($billing_months as $billing_month) {
    // Build the key as float made of Y as interger part and month as decimal part pad with 0 e.g. January 2013 = 2013.01
    // This way we can sort them descending
    $months[(float)$billing_month->getYear() . '.' . str_pad($billing_month->getMonth(), 2, '0', STR_PAD_LEFT)] = $billing_month;
  }

  // Sort billing months by descending order
  krsort($months);

  $opts = array();
  foreach ($months as $billing_month) {
    $opts[$billing_month->getMonthEnum() . '-' . $billing_month->getYear()] = ucfirst(strtolower($billing_month->getMonthEnum())) . ' ' . $billing_month->getYear() . ' - ' . $billing_month->getStatus();
  }

  $form = array();

  $form['#attributes']['class'][] = 'form-horizontal';

  $form['billing_month'] = array(
    '#title' => t('Billing Month'),
    '#type' => 'select',
    '#options' => $opts,
    '#attributes' => array('onchange' => 'javascript: jQuery("#devconnect-monetization-billing-document-form").submit();'),
  );


  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'list',
    '#attributes' => array('style' => 'display:none;'),
  );

  if (isset($form_state['data'])) {
    $form['#billing_docs'] = $form_state['data'];
    $form['#submited'] = TRUE;
  }
  else if (count($opts) > 0) {
    list($default_billing_month) = array_keys($opts);
    list($month, $year) = explode('-', $default_billing_month);

    //@TODO Remove FALSE parameter once confirmation of field values is right
    $form['#billing_docs'] = $billing_document->getBillingDocuments($developer_id, strtoupper($month), $year, TRUE);
    $form['#submited'] = FALSE;
  }

  $form['#submit'][] = 'devconnect_monetization_billing_document_form_submit';
  return $form;
}

function devconnect_monetization_billing_document_form_submit(&$form, &$form_state) {

  $developer_id = _devconnect_monetization_get_developer_id();
  $client = devconnect_monetization_default_api_client();

  list($month, $year) = explode('-', $form_state['values']['billing_month']);
  $billing_document = new BillingDocument($client);
  //@TODO Remove FALSE parameter once confirmation of field values is right
  $billing_docs = $billing_document->getBillingDocuments($developer_id, strtoupper($month), $year, TRUE);

  $form_state['rebuild'] = TRUE;
  $form_state['data'] = $billing_docs;
  $form_state['redirect'] = array('users/me/monetization/billing', array('fragment' => 'tab3'));
}

function devconnect_monetization_billing_get_billing_document($doc_id) {
  try {
    $doc_id = rawurldecode($doc_id);
    $client = devconnect_monetization_default_api_client();
    $doc = new BillingDocument($client);
    $doc->load($doc_id);
    $file = $doc->getFile();

    header('Content-Disposition: attachment; filename="Billing_Docuemnt-' . $doc->getDocumentNumber() . '_' . time() . '.htm"');
    header('Content-Type: text/html; charset=utf-8');
    header('Content-Length: ' . strlen($file));
    header('Connection: close');

    echo (string)$file;
    die();
  }
  catch (Exception $e) {
    drupal_set_message(t('Error while trying to serve your Billing Document. Please try again later.'), 'error');
    watchdog_exception('devconnect_monetization', $e, "Error: " . $e->getMessage(), NULL, WATCHDOG_ERROR);
    drupal_goto('users/me/monetization/billing', array('fragment' => 'tab3'));
  }
}
