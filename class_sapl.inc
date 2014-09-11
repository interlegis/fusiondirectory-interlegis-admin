<?php
/*
 * This code is part of FusionDirectory (http://www.fusiondirectory.org)
 * Copyright (C) 2012-2013 FusionDirectory
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

class sapl extends simplePlugin
{
  var $mainTab = TRUE;
  var $objectclasses = array('top', 'sapl');

  /* Return plugin informations for acl handling  */
  public static function plInfo()
  {
    return array(
      'plShortName'   => _('SAPL'),
      'plDescription' => _('Sistema de Apoio ao Processo Legislativo'),
      'plSelfModify'  => FALSE,
      'plCategory'    => array('Interlegis'),
      'plObjectType'  => array('sapl' => array(
        'name'        => _('SAPL'),
        'filter'      => 'objectClass=sapl',
        'aclCategory' => 'Interlegis',
        'icon'        => 'plugins/interlegis/images/iconMiniSAPL.png',
        'ou'          => 'ou=interlegis,',
      )),

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
        'name'  => _('SAPL'),
        'attrs' => array(
          new BaseSelectorAttribute ("ou=interlegis,"),
          new HostNameAttribute (_('Name'), _('Entity name for this SAPL.'), 'cn', TRUE, "sapl"),
          new StringAttribute (_('City'),_('Name of the city'), 'city', TRUE ),
          new MailAttribute (
              _('Admin e-mail'), _('Mail address of the SAPL administrator.'),
              'mail', TRUE
          ),
          new StringAttribute ( _('Interlegis code'), _('3 letter unique Interlegis code.'), 'interlegisCode', TRUE , '' ),
          new IntAttribute ( _('Zope instance Group'), _('Zope instance Group for the SAPL.'), 'instanceGroup', TRUE , 0, 99),
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
  }

  /* Returns the dn of the entry */
  function compute_dn()
  {
    return 'cn='.$this->cn.',ou=interlegis,'.$this->base;
  }
}

?>
