<?php
namespace Apigee\Commerce;

use \Apigee\Exceptions\ParameterException as ParameterException;
use \Apigee\Util\Log as Log;

class Limit extends Base\BaseObject {

  /**
   * Limit id
   * @var string
   */
  private $id;

  /**
   * Limit Name
   * @var string
   */
  private $name;

  /**
   * Organization
   * @var \Apigee\Commerce\Organization
   */
  private $organization;

  /**
   * Organization
   * @var string
   */
  private $sub_organization;

  /**
   * DeveloperCategory // Note: Currently not supported
   * @var string
   */
  private $developer_category;

  /**
   * Developer
   * @var string
   */
  private $developer;

  /**
   * MonetizationPackage
   * @var  string
   */
  private $monetization_package;

  /**
   * Product
   * @var string
   */
  private $product;

  /**
   * Application
   * @var string
   */
  private $application;

  /**
   * User is from product custom attributes
   * This can be any product custom attribute which
   * is unique across an org. For now it is just user...
   * @var string
   */
  private $user_id;

  /**
   * com.apigee.commerce.model.Developer.BillingType
   * @var string
   */
  private $developer_billing_type;

  /**
   * Quota limit (e.g. $100 in USD or 1000 as number of transactions)
   * @var double
   */
  private $quota_limit;

  /**
   * Currency
   * @var string
   */
  private $currency;

  /**
   * QuotaType
   * @var string
   */
  private $quota_type;

  /**
   * QuotaPeriodType
   * @var string
   */
  private $quota_period_type;

  /**
   * Duration
   * @var int
   */
  private $duration;

  /**
   * Duration units
   * @var string
   */
  private $duration_type;

  /**
   * If limit is published (i.e. final)
   * @var bool
   */
  private $published;

  /**
   * If limit has reached, halt call execution
   * @var bool
   */
  private $halt_execution;

  public function __construct(\Apigee\Util\APIClient $client) {
    $this->init($client);

    $this->base_url = '/commerce/organizations/' . rawurlencode($client->getOrg()) . '/limits';
    $this->wrapper_tag = 'limit';
    $this->id_field = 'id';
    $this->id_is_autogenerated = FALSE;

    $this->initValues();
  }

  public function getDeveloperLimits($developer_id, $package_id = NULL, $halt = NULL) {
    $query = 'dev=' . rawurlencode($developer_id) . (isset($package_id) ? '&pkg=' . rawurlencode($package_id) : '');
    $query .= isset($halt) ? '&halt=' . ($halt ? 'true' : 'false') : '';
    $url = '/commerce/organizations/' . rawurlencode($this->client->getOrg()) . '/limits?' . $query;
    $this->client->get($url);

    $response = $this->client->getResponse();
    $limits = array();
    foreach ($response[$this->wrapper_tag] as $limit_item) {
      $limit = new Limit($this->client);
      $limit->loadFromRawData($limit_item);
      $limits[] = $limit;
    }
    return $limits;
  }

  /**
   * Implements BaseObject::instantiate_new()
   *
   * @return \Apigee\Commerce\Limit
   */
  public function instantiateNew() {
    return new Limit($this->client);
  }

  /**
   * Implements BaseObject::init_values()
   */
  protected function initValues() {
    $this->name = NULL;
    $this->organization = NULL;
    $this->sub_organization = NULL;
    $this->developer_category = NULL;
    $this->developer = NULL;
    $this->monetization_package = NULL;
    $this->product = NULL;
    $this->application = NULL;
    $this->user_id = NULL;
    $this->developer_billing_type = NULL;
    $this->quota_limit = 0;
    $this->currency = NULL;
    $this->quota_type = NULL;
    $this->quota_period_type = NULL;
    $this->duration = 0;
    $this->duration_type = NULL;
    $this->published = FALSE;
    $this->halt_execution = FALSE;
  }

  /**
   * Implements BaseObject::load_from_raw_data($data, $reset = FALSE)
   *
   * @param array $data
   * @param bool $reset
   */
  public function loadFromRawData($data, $reset = FALSE) {
    if ($reset) {
      $this->initValues();
    }

    if (isset($data['organization'])) {
      $organization = new Organization($this->client);
      $organization->loadFromRawData($data['organization']);
      $this->organization = $organization;
    }
    $excluded_properties = array('organization');
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
  }

  /**
   * Implements BaseObject::__toString()
   */
  public function __toString() {
    // @TODO Verify
    $obj = array();
    $excluded_properties = array_keys(get_class_vars(get_parent_class($this)));
    $properties = array_keys(get_object_vars($this));
    foreach ($properties as $property) {
      if (in_array($property, $excluded_properties)) {
        continue;
      }
      if (isset($this->$property)) {
        if (is_object($this->$property)) {
          $obj[$property] = json_decode((string) $this->$property, TRUE);
        }
        else {
          $obj[$property] = $this->$property;
        }
      }
    }
    return json_encode($obj);
  }

  // accessors(getters/setters)

  /**
   * Get Limit id
   *
   * @return string
   */
  public function getId() {
    return $this->id;
  }

  /**
   * Get Limit Name
   *
   * @return string
   */
  public function getName() {
    return $this->name;
  }

  /**
   * Get Organization
   *
   * @return \Apigee\Commerce\Organization
   */
  public function getOrganization() {
    return $this->organization;
  }

  /**
   * Get SubOrganization name
   *
   * @return string
   */
  public function getSubOrganization() {
    return $this->sub_organization;
  }

  /**
   * Get DeveloperCategory // Note: Currently not supported
   *
   * @return string
   */
  public function getDeveloperCategory() {
    return $this->developer_category;
  }

  /**
   * Get Developer
   *
   * @return string
   */
  public function getDeveloper() {
    return $this->developer;
  }

  /**
   * Get MonetizationPackage
   *
   * @return  string
   */
  public function getMonetizationPackage() {
    return  $this->monetization_package;
  }

  /**
   * Get Product
   *
   * @return string
   */
  public function getProduct() {
    return $this->product;
  }

  /**
   * Get Application
   *
   * @return string
   */
  public function getApplication() {
    return $this->application;
  }

  /**
   * Get User id. User is from product custom attributes
   * This can be any product custom attribute which
   * is unique across an org. For now it is just user...
   *
   * @return string
   */
  public function getUserId() {
    return $this->user_id;
  }

  /**
   * Get com.apigee.commerce.model.Developer.BillingType
   *
   * @return string
   */
  public function getDeveloperBillingType() {
    return $this->developer_billing_type;
  }

  /**
   * Get Quota limit (e.g. $100 in USD or 1000 as number of transactions)
   *
   * @return double
   */
  public function getQuotaLimit() {
    return $this->quota_limit;
  }

  /**
   * Get Currency
   *
   * @return string
   */
  public function getCurrency() {
    return $this->currency;
  }

  /**
   * Get QuotaType
   *
   * @return string
   */
  public function getQuotaType() {
    return $this->quota_type;
  }

  /**
   * Get QuotaPeriodType
   *
   * @return string
   */
  public function getQuotaPeriodType() {
    return $this->quota_period_type;
  }

  /**
   * Get Duration
   *
   * @return int
   */
  public function getDuration() {
    return $this->duration;
  }

  /**
   * Get Duration units
   *
   * @return string
   */
  public function getDurationType() {
    return $this->duration_type;
  }

  /**
   * Get if limit is published (i.e. final)
   *
   * @return bool
   */
  public function isPublished() {
    return $this->published;
  }

  /**
   * Get if limit has reached, halt call execution
   *
   * @return bool
   */
  public function getHaltExecution() {
    return $this->halt_execution;
  }

  /**
   * Set Limit id
   *
   * @param string $id
   */
  public function setId($id) {
    $this->id = $id;
  }

  /**
   * Set Limit Name
   *
   * @param string $name
   */
  public function setName($name) {
    $this->name = $name;
  }

  /**
   * Set Organization
   *
   * @param \Apigee\Commerce\Organization $organization
   */
  public function setOrganization($organization) {
    $this->organization = $organization;
  }

  /**
   * Set SubOrganization name
   *
   * @param string $sub_organization
   */
  public function setSubOrganization($sub_organization) {
    $this->sub_organization = $sub_organization;
  }

  /**
   * Set DeveloperCategory // Note: Currently not supported
   *
   * @param string $developer_category
   */
  public function setDeveloperCategory($developer_category) {
    $this->developer_category = $developer_category;
  }

  /**
   * Set Developer
   *
   * @param string $developer
   */
  public function setDeveloper($developer) {
    $this->developer = $developer;
  }

  /**
   * Set MonetizationPackage
   *
   * @param string $monetization_package
   */
  public function setMonetizationPackage($monetization_package) {
     $this->monetization_package = $monetization_package;
  }

  /**
   * Set Product
   *
   * @param string $product
   */
  public function setProduct($product) {
    $this->product = $product;
  }

  /**
   * Set Application
   *
   * @param string $application
   */
  public function setApplication($application) {
    $this->application = $application;
  }

  /**
   * Set User id. User is from product custom attributes
   * This can be any product custom attribute which
   * is unique across an org. For now it is just user...
   *
   * @param string $user_id
   */
  public function setUserId($user_id) {
    $this->user_id = $user_id;
  }

  /**
   * Set com.apigee.commerce.model.Developer.BillingType
   *
   * @param string $developer_billing_type
   */
  public function setDeveloperBillingType($developer_billing_type) {
    $this->developer_billing_type = $developer_billing_type;
  }

  /**
   * Set Quota limit (e.g. $100 in USD or 1000 as number of transactions)
   *
   * @param double $quota_limit
   */
  public function setQuotaLimit($quota_limit) {
    $this->quota_limit = $quota_limit;
  }

  /**
   * Set Currency
   *
   * @param string $currency
   */
  public function setCurrency($currency) {
    $this->currency = $currency;
  }

  /**
   * Set QuotaType
   *
   * @param string $quota_type Allowed values:
   * [Transactions|CreditLimit|SpendLimit|FeeExposure|Balance]
   * @throws \Apigee\Exceptions\ParameterException
   */
  public function setQuotaType($quota_type) {
    if (!in_array($quota_type, array('Transactions', 'CreditLimit', 'SpendLimit', 'FeeExposure', 'Balance'))) {
      throw new ParameterException('Invalid Quota Type value: ' . $quota_type);
    }
    $this->quota_type = $quota_type;
  }

  /**
   * Set QuotaPeriodType
   *
   * @param string $quota_period_type Allowed values: [CALENDAR|USAGE_START|ROLLING]
   * @throws \Apigee\Exceptions\ParameterException
   */
  public function setQuotaPeriodType($quota_period_type) {
    if (!in_array($quota_period_type, array('CALENDAR', 'USAGE_START', 'ROLLING'))) {
      throw new ParameterException('Invalid Quota Period Type value: ' . $quota_period_type);
    }
    $this->quota_period_type = $quota_period_type;
  }

  /**
   * Set Duration
   *
   * @param int $duration
   */
  public function setDuration($duration) {
    $this->duration = $duration;
  }

  /**
   * Set Duration units
   *
   * @param string $duration_type Allowed values [DAY|WEEK|MONTH|QUARTER|YEAR]
   * @throws \Apigee\Exceptions\ParameterException
   */
  public function setDurationType($duration_type) {
    $duration_type = strtoupper($duration_type);
    if (!in_array($duration_type, array('DAY', 'WEEK', 'MONTH', 'QUARTER', 'YEAR'))) {
      throw new ParameterException('Invalid duration type: ' . $duration_type);
    }
    $this->duration_type = $duration_type;
  }

  /**
   * Set if limit is published (i.e. final)
   *
   * @param bool $published
   */
  public function setPublished($published) {
    $this->published = $published;
  }

  /**
   * Set if limit has reached, halt call execution
   *
   * @param bool $halt_execution
   */
  public function setHaltExecution($halt_execution) {
    $this->halt_execution = $halt_execution;
  }
}
