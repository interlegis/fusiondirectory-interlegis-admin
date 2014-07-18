<?php
/*
 *  Interlegis class portalModelo
 *  Author: Fabio Kaiser Rauber
 *  E-mail: fabiorauber@gmail.com
 */
require_once("interlegis-common.inc");

class portalModelo extends simplePlugin
{
  var $mainTab = TRUE;
  var $objectclasses = array('top', 'portalModelo');

  /* Return plugin informations for acl handling  */
  public static function plInfo()
  {
    return array(
      'plShortName'   => _('Portal Modelo'),
      'plDescription' => _('Portal Modelo'),
      'plSelfModify'  => FALSE,
      'plCategory'    => array('interlegis'),
      'plObjectType'  => array('portalModelo' => array('name'  => _('Portal Modelo'),
                                                       'filter'=> 'objectClass=portalModelo',
                                                       'icon'  => 'plugins/interlegis/images/iconMiniPortalModelo.png' 
                                                 )
                              ),

      'plProvidedAcls' => parent::generatePlProvidedAcls(self::getAttributesInfo())
    );
  }

  // The main function : information about attributes
  static function getAttributesInfo ()
  {
    global $config;
    return array(
      // Attributes are grouped by section
      'main' => array(
        'name'  => _('Portal Modelo'),
        'attrs' => array(
          new BaseSelectorAttribute ("ou=interlegis,"),
          new HostNameAttribute (_('Name'), _('Entity name for this Portal.'), 'cn', TRUE, "www"),
          new StringAttribute (_('City'),_('Name of the city'), 'city', TRUE ),
          new MailAttribute (
              _('Admin e-mail'), _('Mail address of the portal administrator.'),
              'mail', TRUE
          ),
          new StringAttribute ( _('Interlegis code'), _('3 letter unique Interlegis code.'), 'interlegisCode', TRUE , '' ),
          new IntAttribute ( _('Zope instance Group'), _('Zope instance Group for the Portal Modelo.'), 'instanceGroup', TRUE , 0, 99),
	  new DateAttribute (_('Activation Date'), _('Activation Date for the Interlegis Product.'), 'activationDate', TRUE, "U"),
          new TextAreaAttribute (
            _('Remarks'), _('Remarks for the Portal creation.'),
            'description', FALSE
          )
        )
      ),
    );
  }

  function __construct (&$config, $dn = NULL, $object = NULL)
  {
    parent::__construct($config, $dn, $object);

    $this->attributesInfo['main']['attrs']['cn']->setUnique(TRUE);
    $this->attributesAccess['interlegisCode']->setDisabled(TRUE);
    $this->attributesAccess['activationDate']->setDisabled(TRUE);
    $this->attributesAccess['instanceGroup']->setDisabled(TRUE);

    $ldap = $config->get_ldap_link();
    $ldap->cd($config->current['BASE']);
    $cityName = $ldap->get_attribute($this->base,"description");
    $this->attributesAccess['city']->setValue($cityName);

    // preenche o código Interlegis
    if ($this->attributesAccess['interlegisCode']->getValue() == '') {
      $this->attributesAccess['interlegisCode']->setValue(generateInterlegisCode($cityName, $ldap));
    }

    // preenche a instancia do Zope
    if ($this->attributesAccess['instanceGroup']->getValue() == '') {
      $this->attributesAccess['instanceGroup']->setValue(decide_instance($ldap,'portalModelo'));
    }


  }

  /* Returns the dn of the entry */
  function compute_dn()
  {
    return 'cn='.$this->cn.',ou=interlegis,'.$this->base;
  }

}

?>
