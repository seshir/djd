<?php
/**
 * @file
 * Parses a WADL file into classes
 *
 * @author Chris Novak cnovak@apigee.com
 */

class WadlParser {

  public static $instance = NULL;

  private function __construct() {

  }

  public static function getInstance() {
    if (!isset(self::$instance)) {
      self::$instance = new WadlParser();
    }
    return self::$instance;
  }


  public function parse($xml_string) {
    libxml_use_internal_errors(true);
    $xml = simplexml_load_string($xml_string);
    if ($xml === false) {
      $error_msg = "Failed loading XML. ";
      foreach (libxml_get_errors() as $error) {
        $error_msg .= $error->message;
      }
      drupal_set_message($error_msg, 'error');
      return FALSE;
    }
    // get resources
    $base_url = (string) $xml->resources['base'];

    $methods = array();
    foreach ($xml->resources->resource as $xml_resource) {
      $resource_path = (string) $xml_resource['path'];

      // Parse the params
      $resource_params = $this->parse_api_parameters($xml_resource);

      // Parse the <method> element.
      foreach($xml_resource->method as $xml_method) {
        $id = (string)$xml_method['id'];
        $name = (string)$xml_method['name'];
        // get apigee:displayName
        $display_name = (string)$xml_method->attributes('apigee', TRUE)->displayName;
        $methods[$id] = new ApiMethod($name, $id, $display_name);
        $methods[$id]->base_url = $base_url;
        $methods[$id]->path = $resource_path;
        // Get the apigee:tags primary value
        $xml_method->registerXPathNamespace('a', 'http://api.apigee.com/wadl/2010/07/');
        foreach($xml_method->xpath("a:tags/a:tag[@primary='true']") as $tag) {
          $methods[$id]->tag = (string)$tag;
        }

        // apigee:authentication tag
        $xml_authentication = $xml_method->xpath("a:authentication/@required");
        if($xml_authentication) {
          $methods[$id]->is_authentication_needed = (string)$xml_authentication[0]['required'];
        }

        // example element is used to specify the sample URL to display in the Console's request URL field. -->
        // Note: This is not used by the new Console
        $xml_example_url = $xml_method->xpath("a:example/@url");
        if($xml_example_url) {
          $methods[$id]->example_url = (string)$xml_example_url[0]['url'];
        }

        // The content of the doc element is shown as a tooltip in the Console's method list.
        $xml_doc = $xml_method->doc;
        if($xml_doc) {
          $xml_doc->registerXPathNamespace('a', 'http://api.apigee.com/wadl/2010/07/');
          $xml_doc_url = $xml_doc->xpath("@a:url");
          if($xml_doc_url) {
            $methods[$id]->doc_url = (string)$xml_doc_url[0]['url'];
          }
          $methods[$id]->doc = trim((string)$xml_doc);
        }

        // parse request tag for more params
        $method_params = $this->parse_api_parameters($xml_method->request);
        // add both request params and method params to this the method params
        $methods[$id]->parameters = array_merge($resource_params,$method_params);

        // The apigee:payload section describes the body content, i.e. the payload.
        // Set attribute required to true to indicate the content as mandatory in the Console
        $xml_representation = $xml_method->request->representation;
        if(!empty($xml_representation)) {
          // representation element can contain params
          $method_params = $this->parse_api_parameters($xml_representation);
          // add both request params and method params to this the method params
          $methods[$id]->parameters = array_merge($methods[$id]->parameters,$method_params);

          $xml_representation->registerXPathNamespace('a', 'http://api.apigee.com/wadl/2010/07/');
          $xml_payload = $xml_representation->xpath("a:payload");
          if($xml_payload) {
            $methods[$id]->representation_payload_is_required = (string)$xml_payload[0]['required'];

            $xml_payload_doc = $xml_payload[0]->doc;
            if(!empty($xml_payload_doc)) {
              $methods[$id]->representation_payload_doc = trim((string)$xml_payload_doc);

              $xml_payload_doc->registerXPathNamespace('a', 'http://api.apigee.com/wadl/2010/07/');
              $xml_payload_doc_url = (string)$xml_payload_doc->attributes('http://api.apigee.com/wadl/2010/07/');
              if($xml_payload_doc_url) {
                $methods[$id]->representation_payload_doc_url = $xml_payload_doc_url;
              }
            }

            $xml_payload[0]->registerXPathNamespace('a', 'http://api.apigee.com/wadl/2010/07/');
            $xml_payload_content = $xml_payload[0]->xpath("a:content");
            if($xml_payload_content) {
              $methods[$id]->representation_payload_content = trim((string)$xml_payload_content[0]);
            }
          }
        }
      }
    }
    return $methods;
  }

  /**
   * Parse the params in either the resource or method
   * @param SimpleXML $root_xml_element - either the resource or method SimpleXml element
   *
   * @return A map with all params, keyed by param name
   */
  private function parse_api_parameters($root_xml_element) {
    $params = array();
    if(!$root_xml_element->param) {
      return $params;
    }
    foreach($root_xml_element->param as $param) {
      // params can be in <resource> or <method><request> tags
      // A "query" style denotes a query parameter. "header" is a common alternative
      $param_obj = new ApiParameter(
        (string)$param['name'],
        (string)$param['required'],
        (string)$param['type'],
        (string)$param['style'],
        (string)$param['default']
      );
      // get the doc element
      if($param->doc) {
        $param_obj->doc = trim((string)$param->doc);
      }
      // get any options
      $options = array();
      foreach($param->option as $option) {
        $option_value = (string)$option['value'];
        $option_media_type = (string)$option['mediaType'];
        $options[$option_value] = $option_media_type;
      }
      $param_obj->options = $options;

      $params[$param_obj->name] = $param_obj;
    }
    return $params;
  }
}

class ApiMethod  {

  public $base_url = NULL;
  public $path = NULL;

  public $displayName = NULL;
  public $id = NULL;
  public $name = NULL;
  public $parameters = array();
  public $tag = NULL;
  public $is_authentication_needed = NULL;
  public $example_url = NULL;
  public $doc_url = NULL;
  public $doc = NULL;

  public $representation_payload_is_required = NULL;
  public $representation_payload_doc = NULL;
  public $representation_payload_doc_url = NULL;
  public $representation_payload_content = NULL;



  public function __construct($name, $id, $displayName) {
    $this->name = $name;
    $this->id = $id;
    $this->displayName = $displayName;
  }


}

class ApiParameter  {

  public $name = NULL;
  public $required = NULL;
  public $type = NULL;
  public $style = NULL;
  public $default = NULL;
  public $doc = NULL;
  // options is a map with key:value and value:mediaType
  public $options = array();

  public function __construct($name, $required, $type, $style, $default) {
    $this->name = $name;
    $this->required = $required;
    $this->type = $type;
    $this->style = $style;
    $this->default = $default;
  }


}