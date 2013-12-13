<?php
namespace Apigee\Exceptions;

class ResponseException extends \Exception {

  private $uri;
  private $params;
  private $responseBody;

  public $responseObj;

  public function __construct($message, $code = 0, $uri = NULL, $params = NULL, $response_body = NULL) {
    parent::__construct($message, $code);
    $this->uri = $uri;
    $this->params = $params;
    $this->responseBody = $response_body;
    $this->responseObj = NULL;
  }

  public function getUri() {
    return $this->uri;
  }
  public function getParams() {
    return $this->params;
  }
  public function getResponse() {
    return $this->responseBody;
  }

  public function __toString() {
    $msg = $this->getMessage();

    if (is_object($this->responseObj) && $this->responseObj instanceof \Apigee\Util\HTTPResponse) {
      $msg .= '<pre>' . (string)$this->responseObj . '</pre>';
    }

    return $msg;
  }
}