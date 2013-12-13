<?php

/**
 * @file
 * Custom modifications for Dev Portal
 * API Management module, Entity related classes.
 *
 * @author
 * Cesar Galindo <cesar@apigee.com>
 *
 * @since 2013-01-Sept
 */

/**
 * Entity controller for API Products
 */
class APIManagementEntityController extends EntityAPIController {

  /**
   * @var
   */
  //protected $service;

  public function __construct($entity_type) {
    parent::__construct($entity_type);
  }

  /**
   * Retrieves Apps in an Org.
   *
   * @return The results in a Traversable object.
   */
  public function query($ids, $conditions, $revision_id = FALSE) {
    global $user;

    // Include the CTools tools that we need.
    ctools_include('ajax');
    ctools_include('modal');

    // Add CTools' javascript to the page. -- this is needed to make module pop ups work
    ctools_modal_add_js();

    $module_path = drupal_get_path('module', 'api_management');
    drupal_add_css("$module_path/api_management.css");
    drupal_add_js("$module_path/api_management.js");

    try {
      module_load_include('module', 'devconnect');
      $client = devconnect_default_api_client();

        try {
          $api_nids = array();
          $api_titles = array();

          module_load_include('php', 'swisscom_developer_apps', 'SwisscomAPIProduct');
          $api_product_obj = new SwisscomAPIProduct($client);

          $user_roles = _api_management_get_api_roles();

          // Get list of API Products stored in Drupal DB
          $query = db_select('node', 'n');
          $query->leftJoin('field_data_api_roles', 'a', 'n.nid = a.entity_id');
          $query->leftJoin('field_data_field_api_display_name', 'd', 'n.nid = d.entity_id');
          $query->leftJoin('field_data_field_api_developer_count', 'c', 'n.nid = c.entity_id');
          $query->fields('n', array('nid', 'title'));
          $query->fields('a', array('api_roles_value'));
          $query->fields('d', array('field_api_display_name_value'));
          $query->fields('c', array('field_api_developer_count_value'));
          $query->condition('n.type', 'api_product');
          $result = $query->execute();

          while ($item = $result->fetchAssoc()) {
            $api_nids[$item['title']] = $item['nid'];
            $api_titles[$item['title']] = $item['field_api_display_name_value'];
            $api_developers[$item['title']] = $item['field_api_developer_count_value'];

            // Process Roles...
            if (isset($api_status[$item['title']])) {
              // Check if user if Anonymous or Authenticated
              if ($item['api_roles_value'] < API_ADMIN) {
                $api_status[$item['title']] = $item['api_roles_value'];
              }
              elseif ($api_status[$item['title']] > 2) {
                if ($item['api_roles_value'] > $api_status[$item['title']]) {
                  $api_status[$item['title']] = $item['api_roles_value'];
                }
              }
            }
            else {
              $api_status[$item['title']] = $item['api_roles_value'];
            }

            if (isset($dc_api_products[$item['title']])) {
              $dc_api_products[$item['title']] .= ', ' . $user_roles[$item['api_roles_value']];
            }
            else {
              $dc_api_products[$item['title']] = $user_roles[$item['api_roles_value']];
            }
          }

          $id = 0;
          $api_products = $api_product_obj->list_all_products();

          foreach ($api_products as $api_product) {
            $id++;
            $api_nid = $api_nids[$api_product->get_name()];

            $entity = new APIManagementEntity;
            $name = $api_product->get_display_name();

            if ($api_nid) {
              $status = $entity->get_status($api_status[$api_product->get_name()]);
              $entity->id = $id . ' ' . $status .'-api';
              $entity->productName = $api_product->get_name();
              $entity->displayName = $name;
              $entity->status = $status;

              if ($api_titles[$api_product->get_name()] != $name) {
                $entity->displayName = $api_titles[$api_product->get_name()];
                $entity->apiMSName = $name;
                $name = $entity->displayName;
              }

              $entity->editUrl = '<a href="/admin/api/edit_api_management/' . $name . '/' . $status . '" class="api-remove-' . $id . '">Edit</a>';

              if ($status == 'Public') {
                $entity->developerCount = t('all');
                $entity->makePublic = '';
                $entity->makePrivate = ctools_modal_text_button(t("Make Private"), "admin/api/nojs/api_management_private_api_form/$name/$id/$api_nid", t('Make Private'),  $class = 'api-remove-'.$id);
                $entity->makeInternal = ctools_modal_text_button(t("Make Internal"), "admin/api/nojs/api_management_internal_api_form/$name/$id/$api_nid", t('Make Internal'),  $class = 'api-remove-'.$id);
              }
              elseif ($status == 'Private') {
                $entity->developerCount = $api_developers[$api_product->get_name()];
                $entity->makePublic = ctools_modal_text_button(t("Make Public"), "admin/api/nojs/api_management_public_api_form/$name/$id/$api_nid", t('Make Public'),  $class = 'api-remove-'.$id);
                $entity->makePrivate = '';
                $entity->makeInternal = ctools_modal_text_button(t("Make Internal"), "admin/api/nojs/api_management_internal_api_form/$name/$id/$api_nid", t('Make Internal'),  $class = 'api-remove-'.$id);
              }
              else {
                $entity->developerCount = $api_developers[$api_product->get_name()];
                $entity->makePublic = ctools_modal_text_button(t("Make Public"), "admin/api/nojs/api_management_public_api_form/$name/$id/$api_nid", t('Make Public'),  $class = 'api-remove-'.$id);
                $entity->makePrivate = ctools_modal_text_button(t("Make Private"), "admin/api/nojs/api_management_private_api_form/$name/$id/$api_nid", t('Make Private'),  $class = 'api-remove-'.$id);
                $entity->makeInternal = '';
              }
              $entity->apiChange = ctools_modal_text_button(t("Change Name"), "admin/api/nojs/api_management_change_api_name_form/$entity->displayName/$id/$api_nid/$entity->apiMSName", t('Change Name'),  $class = 'api-remove-'.$id);
              $entity->delete = ctools_modal_text_button(t("Delete"), "admin/api/nojs/api_management_delete_api_form/$name/$id/$api_nid", t('Delete'),  $class = 'api-remove-'.$id);
            }
            else {
              $displayName =  $api_product->get_name();
              $status = 'new';
              $entity->id = $id . ' new-api';
              $entity->productName = $displayName;
              $entity->displayName = $name;
              $entity->status = t('NEW API IN MANAGEMENT SERVER');
              $entity->apiMSName = '';
              $entity->editUrl = '';
              $entity->makePublic = '';
              $entity->makePrivate = '';
              $entity->makeInternal = ctools_modal_text_button(t("Create API"), "admin/api/nojs/api_management_create_api_form/$name/$id/$displayName", t('Create API'),  $class = 'api-remove-'.$id);
              $entity->apiChange = '';
              $entity->delete = '';
            }

            $entity_part[$status][$id] = $entity;
            unset( $api_nids[$api_product->get_name()]);
          }

          foreach ($api_nids as $name => $nid) {
            $id++;
            $title = $api_titles[$name];

            $entity = new APIManagementEntity;
            $entity->id = $id . ' no-api';
            $entity->productName = $name;
            $entity->displayName = $api_titles[$name];
            $entity->status = $entity->get_status($api_status[$name]) . t(' - API DOES NOT EXIST IN MANAGEMENT SERVER');
            $entity->apiMSName = '';
            $entity->editUrl = '<a href="/admin/api/edit_api_management/' . $title . '/' . $entity->status . '" class="api-remove-' . $id . '">Edit</a>';

            if ($status == 'Public') {
              $entity->developerCount = t('all');
              $entity->makePublic = '';
              $entity->makePrivate = ctools_modal_text_button(t("Make Private"), "admin/api/nojs/api_management_private_api_form/$title/$id/$nid", t('Make Private'),  $class = 'api-remove-'.$id);
              $entity->makeInternal = ctools_modal_text_button(t("Make Internal"), "admin/api/nojs/api_management_internal_api_form/$title/$id/$nid", t('Make Private'),  $class = 'api-remove-'.$id);
            }
            elseif ($status == 'Private') {
              $entity->developerCount = $api_developers[$name];
              $entity->makePublic = ctools_modal_text_button(t("Make Public"), "admin/api/nojs/api_management_public_api_form/$title/$id/$nid", t('Make Public'),  $class = 'api-remove-'.$id);
              $entity->makePrivate = '';
              $entity->makeInternal = ctools_modal_text_button(t("Make Internal"), "admin/api/nojs/api_management_internal_api_form/$title/$id/$nid", t('Make Private'),  $class = 'api-remove-'.$id);
            }
            else {
              $entity->developerCount = $api_developers[$name];
              $entity->makePublic = ctools_modal_text_button(t("Make Public"), "admin/api/nojs/api_management_public_api_form/$title/$id/$nid", t('Make Public'),  $class = 'api-remove-'.$id);
              $entity->makePrivate = ctools_modal_text_button(t("Make Private"), "admin/api/nojs/api_management_private_api_form/$title/$id/$nid", t('Make Private'),  $class = 'api-remove-'.$id);
              $entity->makeInternal = '';
            }
            $entity->apiChange = ctools_modal_text_button(t("Change Name"), "admin/api/nojs/api_management_change_api_name_form/$title/$id/$nid", t('Change Name'),  $class = 'api-remove-'.$id);
            $entity->delete = ctools_modal_text_button(t("Delete"), "admin/api/nojs/api_management_delete_api_form/$title/$id/$nid", t('Delete'),  $class = 'api-remove-'.$id);
            $entity_part['none'][$id] = $entity;
          }

          $entities = array();
          if (isset($entity_part['none'])) {
            $entities = $entities + $entity_part['none'];
          }
          if (isset($entity_part['new'])) {
            $entities = $entities + $entity_part['new'];
          }
          if (isset($entity_part['Public'])) {
            $entities = $entities + $entity_part['Public'];
          }
          if (isset($entity_part['Private'])) {
            $entities = $entities + $entity_part['Private'];
          }
          if (isset($entity_part['Internal'])) {
            $entities = $entities + $entity_part['Internal'];
          }

        }
        catch (Exception $e) {
          drupal_set_message($e->getMessage(), 'error');
        }
    }
    catch (Exception $e) {
      $code = $e->getCode();
      $status = $e->getMessage();
      $error = "STATUS: $code - $status; ";
      $error .= "Communication with the Apigee endpoint is compromised. Cannot get API Products List.";
      if (user_access("administer organization settings")) {
        $error .= "<pre>" . $client->get_response_string() . "</pre>";
      }
      watchdog('api_management', $error, array(), WATCHDOG_ERROR);
      return "<h4>" . $error . "</h4>";
    }


    return $entities;
  }
}

/**
 * The API Product Management entity.
 */
class APIManagementEntity extends Entity {

  protected $entityType = 'api_management';

  /**
   * @var
   */
  protected $entry;

  public function __construct($entry = NULL) {
    // I have nothing to do :(
  }

  /**
   * Returns info about the properties of this entity, as declared in hook_entity_property_info().
   *
   * @return array
   *   The properties defined for this entity.
   *
   * @see api_management_entity_property_info()
   */
  public function properties() {
    $info = entity_get_property_info($this->entityType());
    return $info['properties'];
  }

  /**
   * Returns APIManagement entry object of this entity.
   *
   * @return ...
   */
  public function entry() {
    return $this->entry;
  }

  /**
   * Returns APIManagement entry object of this entity.
   *
   * @return ...
   */
  public function get_status($role) {

    if ($role < API_ADMIN) {
      $status = 'Public';
    }
    elseif ($role == API_ADMIN) {
      $status = 'Internal';
    }
    else {
      $status = 'Private';
    }

    return $status;
  }

}
