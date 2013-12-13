<?php
/**
 * @file
 * Custom modifications for Dev Portal
 *
 * @since 2013-16-Aug
 */

use Apigee\Exceptions\InvalidDataException as InvalidDataException;
use Apigee\Exceptions\ResponseException as ResponseException;
use Apigee\Util\APIClient as APIClient;

class SwisscomSmsToken extends Apigee\ManagementAPI\Base {

  /**
   * @var string
   * There is probably a finite number of possible values, but I haven't found
   * a definitive list yet.
   */
  private $status;
  /**
   * @var string
   */

  public function get_status() {
    return $this->status;
  }

  /**
   * Initializes this object
   *
   * @param \Apigee\Util\APIClient $client
   * @param string $developer
   */
  public function __construct(APIClient $client) {
    $this->init($client);
    // Endpoint is inserted automatically when swisscom_smstoken_apiverify_client is called
    //$this->base_url = variable_get('swisscom_sms_code_endpoint', FALSE);
  }

  /**
   * This function will verify the SMS Pin/Code
   *
   * @return boolean TRUE if token is validated.
   */
  public function validateSmsToken($mobile_number = NULL, $token = NULL) {

    $custom_headers = array('client_id' => variable_get('swisscom_smstoken_apikey', FALSE));

    $url = '/validate/' . $this->urlEncode($mobile_number) . '/' . $this->urlEncode($token);
    $this->client->get($url, 'application/json; charset=utf-8', $custom_headers);
    $response = json_decode($this->getResponse());
    if(isset($response->validateSmsTokenResponse->state)) {
      if($response->validateSmsTokenResponse  ->state = 'OK') {
        return TRUE;
      }
    }
    return FALSE;
  }


  /**
   * This function will generate an SMS pin
   */
  public function smsToken($mobile_number = NULL) {

    $payload = array(
      'text' => variable_get('swisscom_smstoken_message',t('Please enter the following token: %TOKEN% to validate your phone number.')),
      'tokenType' => 'SHORT_ALPHANUMERIC',
      'expireTime' => variable_get('swisscom_smstoken_expiretime',60),
      'tokenLength' => 6,
      'eventCorrelationId' => variable_get('swisscom_smstoken_eventcorrelationid', 'Dev Portal'),
    );

    $custom_headers = array('client_id' => variable_get('swisscom_smstoken_apikey', FALSE),'Host' => variable_get('swisscom_smstoken_host','swisscom.apigee.net'));

    $url = '/sms/' . $this->urlEncode($mobile_number);
    $this->client->post($url, $payload, 'application/json; charset=utf-8', 'application/json; charset=utf-8', $custom_headers);
    $response = json_decode($this->getResponse());
    if(isset($response->sendSmsTokenResponse->state)) {
      if($response->sendSmsTokenResponse->state = 'OK') {
        return TRUE;
      }
    }
    return FALSE;
  }
}
