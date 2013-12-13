<?php

 /**
 * @file
 * API Management module views integration.
 *
 * @author
 * Cesar Galindo <cesar@apigee.com>
 *
 * @since 2013-01-Sept
 */
 
/**
 * Implements hook_views_data_alter().
 *
 * Makes the views table provided by the entity api module usable as base table.
 */
function api_management_views_data_alter(&$data) {
  $data['entity_api_management']['table']['base'] = array(
    'field' => 'id',
    'title' => t('API Management'),
    'help' => t('List API Management entities.'),
    'query class' => 'APIManagementViewsQueryPlugin',
  );
}

/**
 * Implements hook_views_plugins().
 */
function api_management_views_plugins() {
  return array(
    'query' => array(
      'APIManagementViewsQueryPlugin' => array(
        'title' => t('API Management views query plugin'),
        'help' => t('Loads API Management entities.'),
        'handler' => 'APIManagementViewsQueryPlugin',
      ),
    ),
  );
}

/**
 * Query plugin for listing keymanagement entities.
 */
class APIManagementViewsQueryPlugin extends views_plugin_query {
  /**
   * The entity type we are listing.
   */
  public $entityType;

  /**
   * An array of conditions to pass to entity_load().
   */
  public $conditions = array();

  /**
   * Constructor; Create the basic query object and fill with default values.
   */
  public function init($base_table, $base_field, $options) {
    parent::init($base_table, $base_field, $options);
    $table = views_fetch_data($base_table);
    $this->entityType = $table['table']['entity type'];
  }

  public function build(&$view) {
    $this->view = $view;
  }

  /**
   * Executes the query and fills the associated view object with according
   * values.
   *
   * Values to set: $view->result, $view->total_rows, $view->execute_time,
   * $view->pager['current_page'].
   */
  public function execute(&$view) {
    $start = microtime(true);

    // Process in Entity file..
    $view->items_per_page = $view->display_handler->default_display->options['pager']['options']['items_per_page'];

    // Just load all entities.
    $view->result = entity_load($this->entityType, FALSE, $this->conditions);
    $view->total_rows  = $view->result['total_rows']->total_rows;
    unset($view->result['total_rows']);
    $view->execute_time = microtime(true) - $start;
  }

  /**
   * Returns the according entity objects for the given query results.
   */
  public function get_result_entities($results, $relationship = NULL, $field = NULL) {
    return array($this->entityType, $results);
  }

  /**
   * Gets wrappers for the result entities.
   */
  public function get_result_wrappers($results, $relationship = NULL, $field = NULL) {
    $wrappers = array();
    foreach ($results as $id => $entity) {
      $wrappers[$id] = entity_metadata_wrapper($this->entityType, $entity);
    }

     // Apply the relationship, if necessary.
    $selector_suffix = '';
    if ($field && ($pos = strrpos($field, ':'))) {
      $selector_suffix = substr($field, 0, $pos);
    }
    if ($selector_suffix || ($relationship && !empty($this->view->relationship[$relationship]))) {
      // Use EntityFieldHandlerHelper to compute the correct data selector for
      // the relationship.
      $handler = (object) array(
          'view' => $this->view,
          'relationship' => $relationship,
          'real_field' => '',
      );
      $selector = EntityFieldHandlerHelper::construct_property_selector($handler);
      $selector .= ($selector ? ':' : '') . $selector_suffix;
      return EntityFieldHandlerHelper::extract_property_multiple($wrappers, $selector);
    }

    // If no relationship is given, just return the entities.
     return array($this->entityType, $wrappers);
  }
}
