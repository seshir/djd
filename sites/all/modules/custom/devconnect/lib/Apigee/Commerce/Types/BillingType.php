<?php

namespace Apigee\Commerce\Types;

final class BillingType extends Type {

  const PREPAID = 'PREPAID';
  const POSTPAID = 'POSTPAID';
  const BOTH = 'BOTH';

  private function __construct() {
  }

}