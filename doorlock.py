from web3.middleware import geth_poa_middleware
import RPi.GPIO as GPIO
import cryptnoxpy as cp
from web3 import Web3
import time
from time import gmtime,strftime
import gzip
import json
from eth_utils.curried import keccak
from threading import Thread
import os
import sys

#GPIO setup
relay = 18
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.setup(relay,GPIO.OUT)
GPIO.output(relay,0)

infura_url = "https://goerli.infura.io/v3/ac389dd3ded74e4a85cc05c8927825e8"
w3 = Web3(Web3.HTTPProvider(infura_url))
w3.middleware_onion.inject(geth_poa_middleware, layer=0)
contract_address = w3.toChecksumAddress('0xd529A426754A4DBDdAadB428c076B0C733175302')
abi = json.loads('[{"anonymous":false,"inputs":[{"indexed":true,"internalType":"address","name":"account","type":"address"},{"indexed":true,"internalType":"address","name":"operator","type":"address"},{"indexed":false,"internalType":"bool","name":"approved","type":"bool"}],"name":"ApprovalForAll","type":"event"},{"anonymous":false,"inputs":[{"indexed":true,"internalType":"address","name":"previousOwner","type":"address"},{"indexed":true,"internalType":"address","name":"newOwner","type":"address"}],"name":"OwnershipTransferred","type":"event"},{"anonymous":false,"inputs":[{"indexed":true,"internalType":"address","name":"operator","type":"address"},{"indexed":true,"internalType":"address","name":"from","type":"address"},{"indexed":true,"internalType":"address","name":"to","type":"address"},{"indexed":false,"internalType":"uint256[]","name":"ids","type":"uint256[]"},{"indexed":false,"internalType":"uint256[]","name":"values","type":"uint256[]"}],"name":"TransferBatch","type":"event"},{"anonymous":false,"inputs":[{"indexed":true,"internalType":"address","name":"operator","type":"address"},{"indexed":true,"internalType":"address","name":"from","type":"address"},{"indexed":true,"internalType":"address","name":"to","type":"address"},{"indexed":false,"internalType":"uint256","name":"id","type":"uint256"},{"indexed":false,"internalType":"uint256","name":"value","type":"uint256"}],"name":"TransferSingle","type":"event"},{"anonymous":false,"inputs":[{"indexed":false,"internalType":"string","name":"value","type":"string"},{"indexed":true,"internalType":"uint256","name":"id","type":"uint256"}],"name":"URI","type":"event"},{"inputs":[{"internalType":"address","name":"account","type":"address"},{"internalType":"uint256","name":"id","type":"uint256"}],"name":"balanceOf","outputs":[{"internalType":"uint256","name":"","type":"uint256"}],"stateMutability":"view","type":"function"},{"inputs":[{"internalType":"address[]","name":"accounts","type":"address[]"},{"internalType":"uint256[]","name":"ids","type":"uint256[]"}],"name":"balanceOfBatch","outputs":[{"internalType":"uint256[]","name":"","type":"uint256[]"}],"stateMutability":"view","type":"function"},{"inputs":[{"internalType":"address","name":"doorUser","type":"address"},{"internalType":"uint256","name":"tokenIdToCheck","type":"uint256"}],"name":"checkDoorAccess","outputs":[{"internalType":"bool","name":"","type":"bool"}],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"uint256","name":"number","type":"uint256"}],"name":"checkEvenOdd","outputs":[{"internalType":"bool","name":"","type":"bool"}],"stateMutability":"pure","type":"function"},{"inputs":[{"internalType":"address","name":"account","type":"address"},{"internalType":"address","name":"operator","type":"address"}],"name":"isApprovedForAll","outputs":[{"internalType":"bool","name":"","type":"bool"}],"stateMutability":"view","type":"function"},{"inputs":[],"name":"owner","outputs":[{"internalType":"address","name":"","type":"address"}],"stateMutability":"view","type":"function"},{"inputs":[],"name":"renounceOwnership","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"address","name":"","type":"address"},{"internalType":"uint256","name":"","type":"uint256"}],"name":"rentRecords","outputs":[{"internalType":"uint256","name":"","type":"uint256"}],"stateMutability":"view","type":"function"},{"inputs":[{"internalType":"uint256","name":"tokenIdForRent","type":"uint256"}],"name":"rentToken","outputs":[],"stateMutability":"payable","type":"function"},{"inputs":[{"internalType":"address","name":"from","type":"address"},{"internalType":"address","name":"to","type":"address"},{"internalType":"uint256[]","name":"ids","type":"uint256[]"},{"internalType":"uint256[]","name":"amounts","type":"uint256[]"},{"internalType":"bytes","name":"data","type":"bytes"}],"name":"safeBatchTransferFrom","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"address","name":"from","type":"address"},{"internalType":"address","name":"to","type":"address"},{"internalType":"uint256","name":"id","type":"uint256"},{"internalType":"uint256","name":"amount","type":"uint256"},{"internalType":"bytes","name":"data","type":"bytes"}],"name":"safeTransferFrom","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"address","name":"operator","type":"address"},{"internalType":"bool","name":"approved","type":"bool"}],"name":"setApprovalForAll","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"bytes4","name":"interfaceId","type":"bytes4"}],"name":"supportsInterface","outputs":[{"internalType":"bool","name":"","type":"bool"}],"stateMutability":"view","type":"function"},{"inputs":[{"internalType":"address","name":"newOwner","type":"address"}],"name":"transferOwnership","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"uint256","name":"","type":"uint256"}],"name":"uri","outputs":[{"internalType":"string","name":"","type":"string"}],"stateMutability":"view","type":"function"},{"inputs":[],"name":"withdraw","outputs":[],"stateMutability":"nonpayable","type":"function"}]')
contract = w3.eth.contract(address=contract_address, abi=abi)

def address(public_key: str) -> str:
    return keccak(hexstr=("0x" + public_key[2:]))[-20:].hex()

def checksum_address(public_key: str) -> str:
    return Web3.toChecksumAddress(address(public_key))

print(f'{strftime("%Y-%m-%d %H:%M:%S", gmtime())}--Script started. Searching for card...')
while True:
    time.sleep(0.5)
    try:
        card = cp.factory.get_card(cp.Connection())
        print(f'{strftime("%Y-%m-%d %H:%M:%S", gmtime())}--Getting public key')
        public_key = card.get_public_key()
        print(f'{strftime("%Y-%m-%d %H:%M:%S", gmtime())}--Get checksum')
        wallet_address = checksum_address(public_key)
        print(f'{strftime("%Y-%m-%d %H:%M:%S", gmtime())}--Checking if {wallet_address} has access')
        valid_owner = contract.functions.checkDoorAccess(wallet_address,1).call()
        if valid_owner:
            print(f'{strftime("%Y-%m-%d %H:%M:%S", gmtime())}--{wallet_address} has token rented. Allowing access.')
            GPIO.output(relay,1)
            time.sleep(2)
            GPIO.output(relay,0)
            break
        else:
            print(f'{strftime("%Y-%m-%d %H:%M:%S", gmtime())}--{wallet_address} does not have token rented or has expired. Access restricted.')
            GPIO.output(relay,0)
    except KeyboardInterrupt:
        print(f'{strftime("%Y-%m-%d %H:%M:%S", gmtime())}--Terminating script.')
        GPIO.cleanup()
    except Exception as e:
        if 'no card' in str(e):
            pass
        elif 'Connection' in str(e):
            print(f'{strftime("%Y-%m-%d %H:%M:%S", gmtime())}--Connection issue')
        elif 'no seed' in str(e):
            print(f'{strftime("%Y-%m-%d %H:%M:%S", gmtime())}--Card has no seed, please generate or load keys first.')
        else:
            print(f'{strftime("%Y-%m-%d %H:%M:%S", gmtime())}--Exception ocurred, please try again.\n\nError information:\n {e}')
            break
print(f'{strftime("%Y-%m-%d %H:%M:%S", gmtime())}--Script stopped.')
    




