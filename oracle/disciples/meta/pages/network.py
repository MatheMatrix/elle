# -*- encoding: utf-8 -*-

import json
import pymongo.objectid
import traceback
import web

import metalib

from meta.page import Page
from meta import database, conf

class Network(Page):
    """
    Return all user's network ids
        GET /networks
            -> {
                'networks': [network_id1, ...]
            }

    Return one user network
        GET /network/id1
            -> {
                '_id': "id",
                'name': "pretty name",
                'model': 'slug',
                'devices': [device_id1, ...],
                'users': [user_id1, ...],
                'root_block': "base64 string of the root block"
                'descriptor': "base64 string of descriptor",
            }

    Create a new network
        POST /network {
            'name': 'pretty name', # required
            'users': [user_id1, ...], # optional
            'devices': [device_id1, ...], # optional
        }
            -> {
                'success': True,
                'created_network_id': "id",
            }

    Update an existing network
        POST /network {
            '_id': "id",
            'name': 'pretty name', # optional
            'users': [user_id1, ...], # optional
            'devices': [device_id1, ...], # optional
            'root_block': "base64 root block", # optional (implies root_address)
            'root_address': "base64 root address", # optional (implies root_block)
        }
            -> {
                'success': True,
                'updated_network_id': "id",
                'descriptor' : "descriptor file", # only if root_block and root_address where given
            }

    Delete a network
        DELETE /network/id
            -> {
                'success': True,
                'deleted_network_id': "id",
            }
    """

    def GET(self, id=None):
        self.requireLoggedIn()
        if id is None:
            return self.success({'networks': self.user.get('networks', [])})
        else:
            network = database.networks.find_one({
                '_id': pymongo.objectid.ObjectId(id),
                'owner': self.user['_id'],
            })
            network.pop('owner')
            return self.success(network)

    def POST(self):
        self.requireLoggedIn()
        network = self.data
        if '_id' in network:
            action = "update"
        else:
            action = "create"

        action_func = {
            'create': self._create,
            'update': self._update,
        }.get(action.lower())

        if action_func is None:
            return self.error("Unknown action '%s'" % str(action))
        return action_func(network)

    def _create(self, network, network_model="slug"):
        if '_id' in network:
            return self.error("An id cannot be specified while creating a network")
        name = network.get('name', '').strip()
        if not self._checkName(name):
            return self.error("You have to provide a valid network name")
        devices = filter(self._checkDevice, map(lambda d: d.strip(), network.get('devices', [])))
        users = filter(self._checkUser, map(lambda d: d.strip(), network.get('users', [])))

        network = {
            'name': name,
            'owner': self.user['_id'],
            'model': 'slug',
            'users': users,
            'devices': devices,
            'descriptor': None,
            'root_block': None,
            'root_address': None,
        }
        id = database.networks.insert(network)
        assert id is not None
        self.user.setdefault('networks', []).append(str(id))
        database.users.save(self.user)
        return self.success({
            'created_network_id': id
        })

    def _update(self, network):
        id = database.ObjectId(network['_id'])
        if id not in self.user['networks']:
            print "user %s has no network %s in his networks" % (self.user['_id'], id), self.user['networks']
            raise web.Forbidden("The network '"+id+"' does not belong to you")
        to_save = database.networks.find_one({'_id': id})
        if 'name' in network:
            name = network['name'].strip()
            if not self._checkName(name):
                return self.error("Given network name is not valid")
            to_save['name'] = name
        if 'devices' in network:
            devices = filter(self._checkDevice, map(lambda d: d.strip(), network.get('devices', [])))
            to_save['devices'] = devices

        if 'users' in network:
            users = filter(self._checkUser, map(lambda d: d.strip(), network.get('users', [])))
            to_save['users'] = users

        if 'root_block' in network and 'root_address' in network and \
           network['root_block'] and network['root_address']:
            if to_save['root_block'] is not None:
                return self.error("This network has already a root block")

            try:
                root_block = network['root_block']
                root_address = network['root_address']
                assert(network['descriptor'] is None)
                is_valid = metalib.check_root_block_signature(
                    root_block,
                    root_address,
                    self.user['identity_pub']
                )
                if not is_valid:
                    return self.error("The root block was not properly signed")

                to_save['root_block'] = network['root_block']

                to_save['descriptor'] = metalib.generate_network_descriptor(
                    str(id),
                    network['model'],
                    root,
                    conf.INFINIT_AUTHORITY_FILE,
                    conf.INFINIT_AUTHORITY_PASSWORD,
                )
            except Exception, err:
                traceback.print_exc()
                return self.error("Unexpected error: " + str(err))

        id = database.networks.save(to_save)
        return self.success({
            'updated_network_id': id
        })

    def _checkName(self, name):
        return bool(name)
    def _checkDevice(self, device_id):
        """
        device_id is not empty and belongs to the user
        """
        if not device_id:
            return False
        return device_id in self.user.devices

    def _checkUser(self, user_id):
        """
        user_id is not empty and
        is different from the owner and
        exists
        """
        if not user_id:
            return False
        if user_id == str(self.user['_id']):
            return False
        return database.users.find_one({
            '_id': pymongo.objectid.ObjectId(user_id),
        }) is not None

    def DELETE(self, id):
        self.requireLoggedIn()
        try:
            networks = self.user['devices']
            idx = networks.index(id)
            networks.pop(idx)
        except:
            return json.dumps({
                'success': False,
                'error': "The network '%s' was not found" % (id),
            })
        database.users.save(self.user)
        database.networks.find_and_modify({
            '_id': pymongo.objectid.ObjectId(id),
            'owner': self.user['_id'], #not required
        }, remove=True)
        return  self.success({
            'deleted_network_id': id,
        })

