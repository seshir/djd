<?php

/**
 * @file
 * Abstracts the API Product object in the Management API and allows clients
 * to manipulate it.
 *
 * Write support is purely experimental and should not be used unless you're
 * feeling adventurous.
 *
 * @author djohnson
 */

use Apigee\Exceptions\ResponseException as ResponseException;
use Apigee\Util\Cache as Cache;
use Apigee\Util\APIClient as APIClient;

class SwisscomAPIProduct extends Apigee\ManagementAPI\APIProduct {
//class SwisscomAPIProduct extends Apigee\ManagementAPI\Base {

  /**
   * Initializes all member variables
   *
   * @param \Apigee\Util\APIClient $client
   */
  public function __construct(\Apigee\Util\APIClient $client) {
    parent::__construct($client);
  }

  /**
   * Returns a detailed list of all products. This list may have been cached
   * from a previous call.
   *
   * If $show_nonpublic is TRUE, even API Products which are marked as hidden
   * or internal are returned.
   *
   * @param bool $show_nonpublic
   * @return array
   */
  public function list_products($show_nonpublic = FALSE) {
    $products = $this->get_products_cache();
    if (!$show_nonpublic) {
      foreach ($products as $i => $product) {
        if (!$product->is_public()) {
          unset ($products[$i]);
        }
      }
    }
    return $products;
  }

  /**
   * Returns a detailed list of all products. This list may have been cached
   * from a previous call.
   *
   * All API Products are returned:
   * internal, private, public
   *
   * @return array
   */
  public function list_all_products() {
    $products = $this->get_products_cache();
    return $products;
  }

}