<?php

namespace Apigee\Commerce\Exceptions;
use Apigee\Exceptions\ParameterException;
use Apigee\Exceptions\ResponseException;
{

  class CommerceApiException extends \Exception {

    private $commerce_code;

    private $commerce_message;

    private $contexts;

    const DEVELOPER_LEGAL_COMPANY_NAME_NOT_SPECIFIED = 'commerce.developerLegalCompanyNameNotSpecified';
    const DEVELOPER_ADDRESS_NOT_SPECIFIED = 'commerce.developerAddressNotSpecified';
    const RATE_PLAN_NOT_PUBLISHED = 'commerce.ratePlanNotPublished';
    const START_DATE_EARLIER_THAN_TODAY = 'commerce.startDateEarlierThanToday';
    const DEVELOPER_RATE_PLAN_ACCEPT_ERROR = 'commerce.developerRatePlanAcceptError';
    const START_DATE_EARLIER_THAN_PLAN_START_DATE = 'commerce.startDateEarlierThanPlanStartDate';
    const START_DATE_LATER_THAN_PLAN_END_DATE = 'commerce.startDateLaterThanPlanEndDate';
    const END_DATE_EARLIER_THAN_START = 'commerce.endDateEarlierThanStart';
    const END_DATE_LATER_THAN_PLAN_END_DATE = 'commerce.endDateLaterThanPlanEndDate';
    const DEVELOPER_ALREADY_ACCEPTED_RATE_PLAN_FOR_PRODUCT = 'commerce.developerAlreadyAcceptedRatePlanForProduct';
    const DEVELOPER_ALREADY_ACTIVE_ON_RATE_PLAN = 'commerce.developerAlreadyActiveOnRatePlan';
    const RESOURCE_DOES_NOT_EXIST = 'commerce.resourceDoesNotExist';
    const DEVELOPER_HAS_FOLLOWING_OVERLAP_RATE_PLANS = 'commerce.developerHasFollowingOverlapRatePlans';
    const DEVELOPER_ON_RATE_PLAN_WITH_START_DATE = 'commerce.developerOnRatePlanWithStartDate';
    const PREPAID_DEVELOPER_HAS_NO_BALANCE = 'commerce.prepaidDeveloperHasNoBalance';
    const INSUFFICIENT_FUNDS = 'commerce.insufficientFunds';
    const ONLY_FUTURE_DEVELOPER_RATE_PLAN_CAN_BE_DELETED = 'commerce.onlyFutureDeveloperRatePlanCanBeDeleted';

    /**
     * Hold the exception codes relative to Commerce API
     *
     * @var array
     */
    private static $codes = array(
      self::DEVELOPER_LEGAL_COMPANY_NAME_NOT_SPECIFIED => 'Company name not specified, you need to specify company legal name to be able to accept a plan',
      self::DEVELOPER_ADDRESS_NOT_SPECIFIED => 'Address not specified, you need to specify address in your company profile to be able to accept a plan',
      self::RATE_PLAN_NOT_PUBLISHED => 'Plans cannot be added until they are published.',
      self::START_DATE_EARLIER_THAN_TODAY => 'Start date should not be earlier than today.',
      self::START_DATE_EARLIER_THAN_PLAN_START_DATE => 'You cannot add a plan with a start date earlier than a plan start date.',
      self::START_DATE_LATER_THAN_PLAN_END_DATE => 'You cannot add a rate plan with start date going beyond rate plan end date',
      self::END_DATE_LATER_THAN_PLAN_END_DATE => 'You cannot end a rate plan with end date going beyond rate plan end date',
      self::END_DATE_EARLIER_THAN_START => 'You cannot end a plan with end date earlier than start date',
      self::DEVELOPER_ALREADY_ACCEPTED_RATE_PLAN_FOR_PRODUCT => 'You are adding a plan that overlaps with existing plans/products supported by existing plans.',
      self::DEVELOPER_ALREADY_ACTIVE_ON_RATE_PLAN => 'You are adding a plan that is already active',
      self::DEVELOPER_RATE_PLAN_ACCEPT_ERROR => 'Error accepting rate plan',
      self::RESOURCE_DOES_NOT_EXIST => NULL,
      self::DEVELOPER_HAS_FOLLOWING_OVERLAP_RATE_PLANS => NULL,
      self::DEVELOPER_ON_RATE_PLAN_WITH_START_DATE => NULL,
      self::PREPAID_DEVELOPER_HAS_NO_BALANCE => NULL,
      self::INSUFFICIENT_FUNDS => 'Developer doesnt have sufficient funds to proceed',
      self::ONLY_FUTURE_DEVELOPER_RATE_PLAN_CAN_BE_DELETED => 'Delete operation allowed only on future dated developer rate plan',
    );

    /**
     * Determines if this exception is relative to the Commerce API REST call
     *
     * @param \Apigee\Exceptions\ResponseException $e
     * @return boolean
     */
    public static function isCommerceExceptionCode(ResponseException $e) {
      $error_info = json_decode($e->getResponse());
      return isset($error_info->code) && array_key_exists($error_info->code, self::$codes);
    }

    /**
     * Class constructor
     *
     * @param \Apigee\Exceptions\ResponseException $e
     * @return boolean
     * @throws \Apigee\Exceptions\ParameterException if the exception has not a commerce
     * registered code
     */
    public function __construct($e) {
      parent::__construct($e->getMessage(), $e->getCode(), $e);
      if(!self::isCommerceExceptionCode($e)){
        throw new ParameterException('Not a registered commerce exception.', $e);
      }
      $error_info = json_decode($e->getResponse());
      $this->commerce_code = $error_info->code;
      $this->commerce_message = $error_info->message;
    }

    public function getCommerceCode() {
      return $this->commerce_code;
    }

    /**
     * @return string|null if there is a proper message then it is returned,
     * otherwise NULL is return
     */
    public function getCommerceMessage($response_message = FALSE) {
      if ($this->commerce_code == self::DEVELOPER_HAS_FOLLOWING_OVERLAP_RATE_PLANS) {
        return $this->commerce_message;
      }
      return $response_message ? $this->commerce_code . ': ' . $this->commerce_message : self::$codes[$this->commerce_code];
    }
  }

}