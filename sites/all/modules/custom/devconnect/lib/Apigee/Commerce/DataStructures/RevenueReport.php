<?php

namespace Apigee\Commerce\DataStructures;

use Apigee\Commerce\Developer as Developer;
use Apigee\Commerce\Organization as Organization;

class RevenueReport extends DataStructure {

  private $commerce_criteria;

  private $description;

  private $developer;

  private $id;

  private $name;

  private $organization;

  private $type;

  public function __construct($data = NULL, Developer $developer) {

    $excluded_properties = array('commerceCriteria', 'developer', 'organization');
    if (is_array($data)) {
      $this->loadFromRawData($data, $excluded_properties);
    }

    if (isset($data['commerceCriteria'])) {
      $this->commerce_criteria = new CommerceCriteria($data['commerceCriteria']);
    }

    $this->developer = $developer;

    if (isset($data['organization'])) {
      $organization = new Organization($this->developer->getClient());
      $organization->loadFromRawData($data['organization']);
      $this->organization = $organization;
    }


  }

  public function getCommerceCriteria() {
    return $this->commerce_criteria;
  }
  public function setCommerceCriteria($commerce_criteria) {
    $this->commerce_criteria = $commerce_criteria;
  }

  public function getDescription() {
    return $this->description;
  }
  public function setDescription($description) {
    $this->description = $description;
  }

  public function getDeveloper() {
    return $this->developer;
  }
  public function setDeveloper($developer) {
    $this->developer = $developer;
  }

  public function getId() {
    return $this->id;
  }
  public function setId($id) {
    $this->id = $id;
  }

  public function getName() {
    return $this->name;
  }
  public function setName($name) {
    $this->name = $name;
  }

  public function getOrganization() {
    return $this->organization;
  }
  public function setOrganization($organization) {
    $this->organization = $organization;
  }

  public function getType() {
    return $this->type;
  }
  public function setType($type) {
    $this->type = $type;
  }
}