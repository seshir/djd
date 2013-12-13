<?php

namespace Apigee\Commerce\Types;

class DeveloperStatusType extends Type {
  const ACTIVE = 'ACTIVE';
  const INACTIVE = 'INACTIVE';
  const BLACKLISTED = 'BLACKLISTED';

  private function __construct() {}
}