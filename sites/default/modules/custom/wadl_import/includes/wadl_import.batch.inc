<?php

/**
 * Batch Operation for WADL Nodes (Automatic Option)
 */
function _wadl_import_automatic_batch($form_state, &$context) {
  $limit = 10;
  if (!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = count($form_state['values']['methods']);
  }
  for ($i = 0; $i < $limit; $i++) {
    if ($context['sandbox']['progress'] < $context['sandbox']['max']) {
      $xml_string = wadl_import_get_xmlstring($form_state);
      $methods = wadl_import_wadl_import($xml_string);
      $methods = array_values($methods);
      $term_exists = FALSE;
      $vocabularies = taxonomy_get_vocabularies();
      foreach ($vocabularies as $vocab) {
        if ($vocab->machine_name == 'api_tag') {
          $terms = taxonomy_get_tree($vocab->vid);
          $vid = $vocab->vid;
          foreach ($terms as $term) {
            if ($term->name == $methods[$context['sandbox']['progress']]->tag) {
              $term_exists = TRUE;
              continue;
            }
          }
        }
      }
      if (isset($terms) && isset($vid)) {
        if (!$term_exists) {
          $newterm = new stdClass();
          $newterm->name = $methods[$context['sandbox']['progress']]->tag;
          $newterm->vid = $vid;
          taxonomy_term_save($newterm);
        }
      }
      $vocabularies = taxonomy_get_vocabularies();
      foreach ($vocabularies as $vocab) {
        if ($vocab->machine_name == 'api_tag') {
          $terms = taxonomy_get_tree($vocab->vid);
          foreach ($terms as $term) {
            if ($term->name == $methods[$context['sandbox']['progress']]->tag) {
              $tid = $term->tid;
            }
          }
        }
      }
      $node = new stdClass();
      $node->type = $form_state['storage']['page_one_values']['content_type'];
      node_object_prepare($node);

      if (!empty($methods[$context['sandbox']['progress']]->displayName)) {
        $node->title = $methods[$context['sandbox']['progress']]->displayName;
      }
      else {
        $node->title = $methods[$context['sandbox']['progress']]->id;
      }
      $node->language = LANGUAGE_NONE;
      $node->body[$node->language][0]['value'] = $methods[$context['sandbox']['progress']]->doc;
      $node->body[$node->language][0]['format'] = 'plain_text';

      if (!empty($methods[$context['sandbox']['progress']]->parameters)) {
        if (isset($methods[$context['sandbox']['progress']]->parameters['format'])) {
          if ((bool) $methods[$context['sandbox']['progress']]->parameters['format']->options['xml']) {
            $node->field_formats['und'][0]['value'] = $methods[$context['sandbox']['progress']]->parameters['format']->options['xml'];
          }
          else {
            $node->field_formats['und'][0]['value'] = NULL;
          }
          if ((bool) $methods[$context['sandbox']['progress']]->parameters['format']->options['json']) {
            $node->field_formats['und'][1]['value'] = $methods[$context['sandbox']['progress']]->parameters['format']->options['json'];
          }
          else {
            $node->field_formats['und'][1]['value'] = NULL;
          }
        }
      }
      $node->field_api_method_tag['und'][0]['tid'] = $tid;
      $node->field_path['und'][0]['value'] = $methods[$context['sandbox']['progress']]->path;
      if (!empty($methods[$context['sandbox']['progress']]->parameters)) {
        $api_info = array();
        $i = 0;
        foreach ($methods[$context['sandbox']['progress']]->parameters as $element => $params) {
          if ($element != 'format') {
            $api_info[] = array(
              'parameter' => $methods[$context['sandbox']['progress']]->parameters[$element]->name,
              'required' => $methods[$context['sandbox']['progress']]->parameters[$element]->required,
              'type' => $methods[$context['sandbox']['progress']]->parameters[$element]->type,
              'style' => $methods[$context['sandbox']['progress']]->parameters[$element]->style,
              'default' => $methods[$context['sandbox']['progress']]->parameters[$element]->default,
              'description' => $methods[$context['sandbox']['progress']]->parameters[$element]->doc,
              'rank' => $i,
            );
            $i++;
          }
        }
        $node->field_parameters['und'][0]['value'] = drupal_json_encode($api_info);
      }

      // Set API Name taxonomy term
      $node->field_api_name['und'][0]['tid'] = $form_state['storage']['page_one_values']['api_name_term'];

      node_save($node);
      $context['sandbox']['progress']++; // Used to keep track of how many nodes we've already created.
      $context['results'][] = check_plain($node->title); // Keep a running tab of all of the nodes created.
      $context['message'] = "Created node " . $context['sandbox']['progress'] . " of " . $context['sandbox']['max'];
    }
  }
  if ($context['sandbox']['progress'] < $context['sandbox'] ['max']) {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}

/**
 * Finished Callback for the batch
 */
function _wadl_import_finished_batch($success, $results, $operations) {
  if ($success) {
    $message = format_plural(count($results), 'One node created', '@count nodes created.');
  }
  else {
    $message = t('Finished with an error.');
  }
  drupal_set_message($message);
}