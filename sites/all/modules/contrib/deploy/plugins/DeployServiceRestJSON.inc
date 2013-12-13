<?php

/**
 * Class for deploying to a REST server using JSON
 */
class DeployServiceRestJSON extends DeployServiceRest {

  public function deploy(Traversable $iterator) {
    foreach ($iterator as $entity) {
      $json_data = drupal_json_encode($entity);
      $this->config['headers']['Content-Type'] = 'application/json';

      // TODO: Consider making the resource path configurable.
      $url = $this->config['url'] . '/' . $entity->__metadata['uri'] . '.json';
      // Temporary hack for the built-in PHP 5.4 web server that doesn't route
      // filenames correctly.
      // TODO: See if we can remove the .json suffix for all server. The
      // Content-Type header should be enough in most (all?) cases.
      if (php_sapi_name() == 'cli-server') {
        $url = $this->config['url'] . '/' . $entity->__metadata['uri'];
      }
      $this->httpRequest($url, 'PUT', $json_data);
    }
  }

  public function publish(Traversable $iterator) {
    foreach ($iterator as $entity) {
      $this->config['headers']['Content-Type'] = 'application/json';

      // TODO: Consider making the resource path configurable.
      $url = $this->config['url'] . '/' . $entity->__metadata['uri'] . '/publish';
      //$this->httpRequest($url, 'POST');
    }
  }
}
