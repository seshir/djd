<?php

/**
 * Gets the XML String from the WADL File
 */
function wadl_import_get_xmlstring(&$form_state) {

  if($_SESSION['api_docbuilder_xmlstring']) {
    return $_SESSION['api_docbuilder_xmlstring'];
  }

  $xml_string = NULL;

  if (isset($form_state['storage']['page_one_values'])) {
    $values = &$form_state['storage']['page_one_values'];
    if ($values['file']) {
      $file = unserialize($values['file']);
      if (file_exists($file->uri)) {
        $xml_string = file_get_contents($file->uri);
        unlink($file->uri);
      }
    } else if ($values['code']) {
      $xml_string = trim($values['code']);
    }
  }
  $_SESSION['api_docbuilder_xmlstring'] = $xml_string;

  return $xml_string;
}

/**
 * Import Function to import the wadl files
 */
function wadl_import_wadl_import(&$xml_string) {
  require_once './' . drupal_get_path('module', 'wadl_import') . "/classes/wadl_import.class.php";
  $xml_string = utf8_encode($xml_string);
  $resources = WadlParser::getInstance()->parse($xml_string);
  return $resources;
}