<?php

/**
 * @file
 * Company module, Entity related classes.
 */

/**
 * Entity controller for company.
 */
class SwisscomCompanyEntityController extends EntityAPIController {

  protected $service;

  /**
   * Constructor.
   */
  public function __construct($entity_type) {
    parent::__construct($entity_type);

//    $this->client = swisscom_company_client();
//    $this->service = new Zend_Gdata_Photos($this->client);
  }

  /**
   * Retrieves companies.
   *
   * @return The results in a Traversable object.
   */
  public function query($ids, $conditions, $revision_id = FALSE) {
    if ($ids === FALSE) {
      return $this->getAll();
    }

    $entities = array();
    foreach ($ids as $id) {
      $entity = new $this->entityInfo['entity class']($this->getEntry($id));
      $entities[$entity->id] = $entity;
    }
    return $entities;
  }

  /**
   * Get all albums of our configured user.
   */
  protected function getAll() {
    $entities = array();
    $userFeed = $this->service->getUserFeed(picasa_user_account());
    foreach ($userFeed as $userEntry) {
      $entity = new $this->entityInfo['entity class']($userEntry);
      $entities[$entity->id] = $entity;
    }
    return $entities;
  }

  /**
   * Gets a single album entry by ID.
   */
  protected function getEntry($id) {
    $query = new Zend_Gdata_Photos_AlbumQuery();
    $query->setAlbumId($id);
    $query->setType("entry");
    return $this->service->getAlbumEntry($query);
  }

  public function create(array $values = array()) {
    $entity = new $this->entityInfo['entity class']();
    foreach ($values as $name => $value) {
      $entity->$name = $value;
    }
    $entity->is_new = TRUE;
    return $entity;
  }

  public function save($entity, DatabaseTransaction $transaction = NULL) {
    // Save the company by calling API.
    //$entity->entry()->save();
    // Let other hooks and rules do their thing.
    $this->invoke($entity->is_new ? 'insert' : 'update', $entity);
    $this->resetCache($entity->id);
  }

  public function delete($ids, DatabaseTransaction $transaction = NULL) {
    throw new Exception("Deletion is not supported.");
  }
}

/**
 * The company entity.
 */
class SwisscomCompanyEntity extends Entity {

  protected $entityType = 'swisscom_company';

  protected $entry;

  protected function defaultLabel() {
    return $this->title;
  }
  protected function defaultUri() {
    return array('path' => 'company/' . $this->identifier());
  }

  public function __construct(Zend_Gdata_Photos_PhotoEntry $entry = NULL) {
   // $this->entry = isset($entry) ? $entry : new Zend_Gdata_Photos_PhotoEntry();

//    foreach ($this->properties() as $name => $info) {
//      if (empty($info['getter callback'])) {
//        $picasa_name = isset($info['picasa name']) ? $info['picasa name'] : 'gphoto' . $name;
//        $this->$name = call_user_func(array($this->entry, 'get' . $picasa_name))->getText();
//      }
//    }
  }

  /**
   * Applies any possible changes of the entity to the Gdata entry.
   */
  public function updateEntry() {
    foreach ($this->properties() as $name => $info) {
      if (empty($info['getter callback'])) {
        $picasa_name = isset($info['picasa name']) ? $info['picasa name'] : 'gphoto' . $name;
        call_user_func(array($this->entry, 'set' . $picasa_name), $this->$name);
      }
    }
  }

  /**
   * Returns the remote URL of this resource.
   */
  public function remoteURL() {
    return $this->entry->getId()->getText();
  }

  /**
   * Returns info about the properties of this entity, as declared in hook_entity_property_info().
   *
   * @return array
   *   The properties defined for this entity.
   *
   * @see picasa_entity_property_info()
   */
  public function properties() {
    $info = entity_get_property_info($this->entityType());
    return $info['properties'];
  }

  /**
   * Returns the Zend Gdata API entry object of this entity.
   *
   * @return Zend_Gdata_Photos_AlbumEntry
   */
  public function entry() {
    $this->updateEntry();
    return $this->entry;
  }
}
