<?php
namespace Apigee\Commerce;

use \Apigee\Util\Log as Log;

class BankDetail extends Base\BaseObject {

  private $dev_email;

  private $address;
  private $aban;
  private $account_name;
  private $account_number;
  private $bic;
  private $currency;
  private $iban_number;
  private $id;
  private $name;
  private $sort_code;


  public function __construct($developer_email, \Apigee\Util\APIClient $client) {
    $this->init($client);
    $this->dev_email = $developer_email;
    $this->base_url = '/commerce/organizations/' . rawurlencode($this->client->getOrg()) . '/developers/' . rawurldecode($developer_email) . '/bank-details';
    $this->wrapper_tag = 'bankDetail';
    $this->id_field = 'id';

    $this->initValues();
  }

  protected function initValues() {
    $this->aban = NULL;
    $this->account_name = NULL;
    $this->account_number = NULL;
    $this->address = NULL;
    $this->bic = NULL;
    $this->currency = NULL;
    $this->iban_number = NULL;
    $this->id = NULL;
    $this->name = NULL;
    $this->sort_code = NULL;
  }

  public function instantiateNew() {
    return new BankDetail($this->dev_email, $this->client);
  }

  public function loadFromRawData($data, $reset = FALSE) {
    if ($reset) {
      $this->initValues();
    }
    $excluded_properties = array('address');
    foreach (array_keys($data) as $property) {
      if (in_array($property, $excluded_properties)) {
        continue;
      }

      // form the setter method name to invoke setXxxx
      $setter_method = 'set' . ucfirst($property);

      if (method_exists($this, $setter_method)) {
        $this->$setter_method($data[$property]);
      }
      else {
        Log::write(__CLASS__, Log::LOGLEVEL_NOTICE, 'No setter method was found for property "' . $property . '"');
      }
    }

    if (isset($data['address']) && is_array($data['address']) && count($data['address']) > 0) {
      $this->address = new DataStructures\Address($data['address']);
    }
  }

  public function __toString() {
    $obj = array();
    $properties = array_keys(get_object_vars($this));
    $excluded_properties = array_keys(get_class_vars(get_parent_class($this)));
    foreach ($properties as $property) {
      if (in_array($property, $excluded_properties)) {
        continue;
      }
      if (isset($this->$property)) {
        $obj[$property] = $this->$property;
      }
    }
    return json_encode($obj);
  }

  public function getAban() {
    return $this->aban;
  }
  public function setAban($aban) {
    // TODO: validate
    $this->aban = $aban;
  }
  public function getAccountName() {
    return $this->account_name;
  }
  public function setAccountName($name) {
    $this->account_name = $name;
  }
  public function getAccountNumber() {
    return $this->account_number;
  }
  public function setAccountNumber($num) {
    $this->account_number = $num;
  }
  public function getAddress() {
    return $this->address;
  }
  public function setAddress(DataStructures\Address $addr) {
    $this->address = $addr;
  }
  public function getBic() {
    return $this->bic;
  }
  public function setBic($bic) {
    $this->bic = $bic;
  }
  public function getCurrency() {
    return $this->currency;
  }
  public function setCurrency($curr) {
    // TODO: validate
    $this->currency = $curr;
  }
  public function getIbanNumber() {
    return $this->iban_number;
  }
  public function setIbanNumber($num) {
    // TODO: validate?
    $this->iban_number = $num;
  }
  public function getId() {
    return $this->id;
  }
  // Used in data load invoked by $this->loadFromRawData()
  private function setId($id) {
    $this->id = $id;
  }
  public function getName() {
    return $this->name;
  }
  public function setName($name) {
    $this->name = $name;
  }
  public function getSortCode() {
    return $this->sort_code;
  }
  public function setSortCode($code) {
    // TODO: validate
    $this->sort_code = $code;
  }

}