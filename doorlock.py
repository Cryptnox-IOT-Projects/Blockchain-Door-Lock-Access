from web3.middleware import geth_poa_middleware
import RPi.GPIO as GPIO
import cryptnoxpy as cp
from web3 import Web3
import time
import gzip
import json
from eth_utils.curried import keccak
from threading import Thread
import os
import sys

#GPIO setup
relay = 40
GPIO.setmode(GPIO.BOARD)
GPIO.setup(relay,GPIO.OUT)
GPIO.output(relay,0)

infura_url = "https://ropsten.infura.io/v3/ac389dd3ded74e4a85cc05c8927825e8"
w3 = Web3(Web3.HTTPProvider(infura_url))
w3.middleware_onion.inject(geth_poa_middleware, layer=0)
contract_address = w3.toChecksumAddress('0x52ab8a9Ec6a72fb379cBa86360A8B00b60fd0ABE ')
abi = json.loads('[{"anonymous":false,"inputs":[{"indexed":true,"internalType":"address","name":"account","type":"address"},{"indexed":true,"internalType":"address","name":"operator","type":"address"},{"indexed":false,"internalType":"bool","name":"approved","type":"bool"}],"name":"ApprovalForAll","type":"event"},{"anonymous":false,"inputs":[{"indexed":true,"internalType":"address","name":"previousOwner","type":"address"},{"indexed":true,"internalType":"address","name":"newOwner","type":"address"}],"name":"OwnershipTransferred","type":"event"},{"anonymous":false,"inputs":[{"indexed":true,"internalType":"address","name":"operator","type":"address"},{"indexed":true,"internalType":"address","name":"from","type":"address"},{"indexed":true,"internalType":"address","name":"to","type":"address"},{"indexed":false,"internalType":"uint256[]","name":"ids","type":"uint256[]"},{"indexed":false,"internalType":"uint256[]","name":"values","type":"uint256[]"}],"name":"TransferBatch","type":"event"},{"anonymous":false,"inputs":[{"indexed":true,"internalType":"address","name":"operator","type":"address"},{"indexed":true,"internalType":"address","name":"from","type":"address"},{"indexed":true,"internalType":"address","name":"to","type":"address"},{"indexed":false,"internalType":"uint256","name":"id","type":"uint256"},{"indexed":false,"internalType":"uint256","name":"value","type":"uint256"}],"name":"TransferSingle","type":"event"},{"anonymous":false,"inputs":[{"indexed":false,"internalType":"string","name":"value","type":"string"},{"indexed":true,"internalType":"uint256","name":"id","type":"uint256"}],"name":"URI","type":"event"},{"inputs":[{"internalType":"address","name":"account","type":"address"},{"internalType":"uint256","name":"id","type":"uint256"}],"name":"balanceOf","outputs":[{"internalType":"uint256","name":"","type":"uint256"}],"stateMutability":"view","type":"function"},{"inputs":[{"internalType":"address[]","name":"accounts","type":"address[]"},{"internalType":"uint256[]","name":"ids","type":"uint256[]"}],"name":"balanceOfBatch","outputs":[{"internalType":"uint256[]","name":"","type":"uint256[]"}],"stateMutability":"view","type":"function"},{"inputs":[{"internalType":"address","name":"doorUser","type":"address"},{"internalType":"uint256","name":"tokenIdToCheck","type":"uint256"}],"name":"checkDoorAccess","outputs":[{"internalType":"bool","name":"","type":"bool"}],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"address","name":"account","type":"address"},{"internalType":"address","name":"operator","type":"address"}],"name":"isApprovedForAll","outputs":[{"internalType":"bool","name":"","type":"bool"}],"stateMutability":"view","type":"function"},{"inputs":[],"name":"owner","outputs":[{"internalType":"address","name":"","type":"address"}],"stateMutability":"view","type":"function"},{"inputs":[],"name":"renounceOwnership","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"address","name":"","type":"address"},{"internalType":"uint256","name":"","type":"uint256"}],"name":"rentRecords","outputs":[{"internalType":"uint256","name":"","type":"uint256"}],"stateMutability":"view","type":"function"},{"inputs":[{"internalType":"uint256","name":"tokenIdForRent","type":"uint256"}],"name":"rentToken","outputs":[],"stateMutability":"payable","type":"function"},{"inputs":[{"internalType":"address","name":"from","type":"address"},{"internalType":"address","name":"to","type":"address"},{"internalType":"uint256[]","name":"ids","type":"uint256[]"},{"internalType":"uint256[]","name":"amounts","type":"uint256[]"},{"internalType":"bytes","name":"data","type":"bytes"}],"name":"safeBatchTransferFrom","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"address","name":"from","type":"address"},{"internalType":"address","name":"to","type":"address"},{"internalType":"uint256","name":"id","type":"uint256"},{"internalType":"uint256","name":"amount","type":"uint256"},{"internalType":"bytes","name":"data","type":"bytes"}],"name":"safeTransferFrom","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"address","name":"operator","type":"address"},{"internalType":"bool","name":"approved","type":"bool"}],"name":"setApprovalForAll","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"bytes4","name":"interfaceId","type":"bytes4"}],"name":"supportsInterface","outputs":[{"internalType":"bool","name":"","type":"bool"}],"stateMutability":"view","type":"function"},{"inputs":[{"internalType":"address","name":"newOwner","type":"address"}],"name":"transferOwnership","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"uint256","name":"","type":"uint256"}],"name":"uri","outputs":[{"internalType":"string","name":"","type":"string"}],"stateMutability":"view","type":"function"},{"inputs":[],"name":"withdraw","outputs":[],"stateMutability":"nonpayable","type":"function"}]')
contract = w3.eth.contract(address=contract_address, abi=abi)
admin_address = '0x49cd58A52176B03826216BCAabC58c49Bc5655A1'

try:
    private_key = os.environ['privatekey']
except Exception as e:
    print(f'Please export contract\'s private key as environment variable.\n (export privatekey=\'<Insert key here>\')')
    sys.exit(0)

def address(public_key: str) -> str:
    return keccak(hexstr=("0x" + public_key[2:]))[-20:].hex()

def checksum_address(public_key: str) -> str:
    return Web3.toChecksumAddress(address(public_key))


while True:
    time.sleep(0.5)
    try:
        card = cp.factory.get_card(cp.Connection())
        public_key = card.get_public_key()
        wallet_address = checksum_address(public_key)
        print(f'Checking if {wallet_address} has access')
        valid_owner = contract.functions.checkDoorAccess(wallet_address,1).call()
        if valid_owner == wallet_address:
            print(f'Open door.\n{valid_owner} equals to {wallet_address}')
            GPIO.output(relay,1)
            time.sleep(5)
            GPIO.output(relay,0)
        else:
            print(f'Door stays closed.\n{valid_owner} doesn\'nt equal to {wallet_address}')
            GPIO.output(relay,0)
    except Exception as e:
        if 'no card' in str(e):
            print(f'No card found')
        elif 'Connection' in str(e):
            print(f'Connection issue')
        elif 'no seed' in str(e):
            print(f'Card has no seed, please generate or load keys first.')
        else:
            print(f'Exception ocurred, please try again.\n\nError information:\n {e}')
            break
GPIO.cleanup()




