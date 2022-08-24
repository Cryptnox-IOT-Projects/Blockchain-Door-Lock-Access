import React from "react";
import "./App.css";
import { ConnectButton, Button, useNotification } from "web3uikit";
import logo from "./images/Cryptnox.png";
import { useMoralis } from "react-moralis";
import Web3 from 'web3';

const App = () => {

  const PRIVATE_KEY = "6535f3b70c47d46b2db67e812abc7bca931931f2f3075a8e4ae359079df4ab32";
  const dispatch = useNotification();
  const {isAuthenticated,Moralis} = useMoralis();
  const adminWalletAddress = "0x49cd58A52176B03826216BCAabC58c49Bc5655A1";
  const contractAddress = "0xc41CAb17C653b655030D1Ec5b3c483d3F9e512d0";
  const abi = [{"anonymous":false,"inputs":[{"indexed":true,"internalType":"address","name":"owner","type":"address"},{"indexed":true,"internalType":"address","name":"approved","type":"address"},{"indexed":true,"internalType":"uint256","name":"tokenId","type":"uint256"}],"name":"Approval","type":"event"},{"anonymous":false,"inputs":[{"indexed":true,"internalType":"address","name":"owner","type":"address"},{"indexed":true,"internalType":"address","name":"operator","type":"address"},{"indexed":false,"internalType":"bool","name":"approved","type":"bool"}],"name":"ApprovalForAll","type":"event"},{"anonymous":false,"inputs":[{"indexed":true,"internalType":"address","name":"previousOwner","type":"address"},{"indexed":true,"internalType":"address","name":"newOwner","type":"address"}],"name":"OwnershipTransferred","type":"event"},{"anonymous":false,"inputs":[{"indexed":true,"internalType":"address","name":"from","type":"address"},{"indexed":true,"internalType":"address","name":"to","type":"address"},{"indexed":true,"internalType":"uint256","name":"tokenId","type":"uint256"}],"name":"Transfer","type":"event"},{"inputs":[{"internalType":"address","name":"to","type":"address"},{"internalType":"uint256","name":"tokenId","type":"uint256"}],"name":"approve","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"address","name":"owner","type":"address"}],"name":"balanceOf","outputs":[{"internalType":"uint256","name":"","type":"uint256"}],"stateMutability":"view","type":"function"},{"inputs":[{"internalType":"uint256","name":"tokenToCheck","type":"uint256"}],"name":"checkOwner","outputs":[{"internalType":"address","name":"owner","type":"address"}],"stateMutability":"view","type":"function"},{"inputs":[{"internalType":"uint256","name":"tokenId","type":"uint256"}],"name":"getApproved","outputs":[{"internalType":"address","name":"","type":"address"}],"stateMutability":"view","type":"function"},{"inputs":[{"internalType":"address","name":"owner","type":"address"},{"internalType":"address","name":"operator","type":"address"}],"name":"isApprovedForAll","outputs":[{"internalType":"bool","name":"","type":"bool"}],"stateMutability":"view","type":"function"},{"inputs":[{"internalType":"uint256","name":"tokenToMap","type":"uint256"},{"internalType":"address","name":"owner","type":"address"}],"name":"mapTokenToOwner","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"uint256","name":"","type":"uint256"}],"name":"mappedOwners","outputs":[{"internalType":"address","name":"","type":"address"}],"stateMutability":"view","type":"function"},{"inputs":[{"internalType":"address","name":"recipient","type":"address"}],"name":"mintToken","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[],"name":"name","outputs":[{"internalType":"string","name":"","type":"string"}],"stateMutability":"view","type":"function"},{"inputs":[],"name":"owner","outputs":[{"internalType":"address","name":"","type":"address"}],"stateMutability":"view","type":"function"},{"inputs":[{"internalType":"uint256","name":"tokenId","type":"uint256"}],"name":"ownerOf","outputs":[{"internalType":"address","name":"","type":"address"}],"stateMutability":"view","type":"function"},{"inputs":[{"internalType":"address","name":"","type":"address"},{"internalType":"uint256","name":"","type":"uint256"}],"name":"ownershipRecord","outputs":[{"internalType":"uint256","name":"tokenId","type":"uint256"},{"internalType":"uint256","name":"timeStamp","type":"uint256"},{"internalType":"string","name":"tokenURI","type":"string"}],"stateMutability":"view","type":"function"},{"inputs":[],"name":"renounceOwnership","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"address","name":"from","type":"address"},{"internalType":"address","name":"to","type":"address"},{"internalType":"uint256","name":"tokenToReturn","type":"uint256"}],"name":"returnToken","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"address","name":"from","type":"address"},{"internalType":"address","name":"to","type":"address"},{"internalType":"uint256","name":"tokenId","type":"uint256"}],"name":"safeTransferFrom","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"address","name":"from","type":"address"},{"internalType":"address","name":"to","type":"address"},{"internalType":"uint256","name":"tokenId","type":"uint256"},{"internalType":"bytes","name":"data","type":"bytes"}],"name":"safeTransferFrom","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"address","name":"operator","type":"address"},{"internalType":"bool","name":"approved","type":"bool"}],"name":"setApprovalForAll","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"bytes4","name":"interfaceId","type":"bytes4"}],"name":"supportsInterface","outputs":[{"internalType":"bool","name":"","type":"bool"}],"stateMutability":"view","type":"function"},{"inputs":[],"name":"symbol","outputs":[{"internalType":"string","name":"","type":"string"}],"stateMutability":"view","type":"function"},{"inputs":[{"internalType":"uint256","name":"tokenId","type":"uint256"}],"name":"tokenURI","outputs":[{"internalType":"string","name":"","type":"string"}],"stateMutability":"view","type":"function"},{"inputs":[{"internalType":"address","name":"from","type":"address"},{"internalType":"address","name":"to","type":"address"},{"internalType":"uint256","name":"tokenId","type":"uint256"}],"name":"transferFrom","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"address","name":"newOwner","type":"address"}],"name":"transferOwnership","outputs":[],"stateMutability":"nonpayable","type":"function"}];
  const w3 = new Web3(new Web3.providers.HttpProvider('https://ropsten.infura.io/v3/ac389dd3ded74e4a85cc05c8927825e8'));
  const contract = new w3.eth.Contract(abi,contractAddress);

  const handleNewNotification = () => {
    dispatch({
      type:"error",
      message:"Please connect  your crypto wallet",
      title:"Not Authenticated",
      position:"topL"
    })
  }

  const handleRentNotification = (txhash) => {
    dispatch({
      type:"success",
      message:"NFT has been rented.\nTx hash:\n"+txhash,
      title:"NFT rented",
      position:"topL"
    })
  }

  const handleAccessNotification = (hash) => {
    dispatch({
      type:"success",
      message:"Door access has been mapped.\nTx hash:\n"+hash,
      title:"Access granted",
      position:"topL"
    })
  }

  const handleReturnNotification = (hash) => {
    dispatch({
      type:"success",
      message:"NFT has been returned.\nTx hash:\n"+hash,
      title:"NFT returned",
      position:"topL"
    })
  }

  const handleErrorNotification = (error) => {
    dispatch({
      type:"error",
      message:"Something went wrong:\n"+error,
      title:"Unexpected error",
      position:"topL"
    })
  }

  const sleep = (ms) => {
    return new Promise(resolve => setTimeout(resolve,ms));
  }

  const rentNft = async () => {
    if (!isAuthenticated){
      handleNewNotification();
    }else{
      let userWalletAddress = Moralis.User.current().attributes.ethAddress;
      console.log('Renting NFT to card wallet: '+userWalletAddress);
      const nonce = await w3.eth.getTransactionCount(adminWalletAddress, 'latest');
      var rawTransaction = {
        "from":adminWalletAddress,
        "to":contractAddress,
        "nonce":nonce,
        "gas":500000,
        "maxPriorityFeePerGas":1999999987,
        "data":contract.methods.transferFrom(adminWalletAddress,userWalletAddress,0).encodeABI()
      };
      console.log(rawTransaction);
      try {
        let signedTx = await w3.eth.accounts.signTransaction(rawTransaction, PRIVATE_KEY);
        await w3.eth.sendSignedTransaction(signedTx.rawTransaction,async (err,hash) => {
          if (!err){
            await w3.eth.getTransactionReceipt(hash,async (err,receipt) => { 
              console.log('Receipt callback with value: \n'+err+'\n'+receipt);
              if (!err){
                console.log("Transaction has been mined:\n"+receipt);
                handleRentNotification(hash); 
                console.log('Sleeping now before returning')
                await sleep(30*1000);
                console.log('Rent expired, returning token')
                let returnPromise = new Promise(async function(returnHash,returnError){
                  console.log('Renting NFT to card wallet: '+userWalletAddress);
                  const nonce = await w3.eth.getTransactionCount(adminWalletAddress, 'latest');
                  var rawTransaction = {
                    "from":adminWalletAddress,
                    "to":contractAddress,
                    "nonce":nonce,
                    "gas":500000,
                    "maxPriorityFeePerGas":1999999987,
                    "data":contract.methods.returnToken(userWalletAddress,adminWalletAddress,0).encodeABI()
                  };
                  console.log(rawTransaction);
                  const signPromise = w3.eth.accounts.signTransaction(rawTransaction, PRIVATE_KEY);
                  signPromise.then((signedTx) => {
                    w3.eth.sendSignedTransaction(signedTx.rawTransaction, function(err, hash) {
                      if (!err){
                        returnHash(hash);
                      }else{
                        returnError(err);
                      }
                    });
                  });
                returnPromise.then(
                  function(returnHash){
                    console.log('NFT returned on expiry:\n'+returnHash);
                  },
                  function(returnError){
                    console.log('Error returning NFT on expiry:\n'+returnError);
                  }
                );  
                });
              }else{
                console.log("Something went wrong when submitting your transaction:", err)
                handleErrorNotification(err);
              }
            });
          }else{
            console.log("Something went wrong when submitting your transaction:", err)
            handleErrorNotification(err);
          }
        });
      } catch (error) {
        console.log("Something went wrong when submitting your transaction:", error)
        handleErrorNotification(error);
      }
    }
  }

  const mapDoorAccess = async () => {
    if (!isAuthenticated){
      handleNewNotification();
    }else{
      let userWalletAddress = Moralis.User.current().attributes.ethAddress;
      console.log('Mapping door access to: '+userWalletAddress);
      const nonce = await w3.eth.getTransactionCount(adminWalletAddress, 'latest');
      var rawTransaction = {
        "from":adminWalletAddress,
        "to":contractAddress,
        "nonce":nonce,
        "gas":500000,
        "maxPriorityFeePerGas":1999999987,
        "data":contract.methods.mapTokenToOwner(0,userWalletAddress).encodeABI()
      };
      console.log(rawTransaction);
      let signedTransaction = await w3.eth.accounts.signTransaction(rawTransaction, PRIVATE_KEY);
      try{
        await w3.eth.sendSignedTransaction(signedTransaction.rawTransaction, async function(err, hash) {
          if (!err) {
            console.log("The hash of your transaction is: ", hash, "\n");
            await w3.eth.getTransactionReceipt(hash,(err,receipt) => {
              if (!err){
                console.log("Got transaction receipt: \n"+receipt);
                handleAccessNotification(hash); 
              }else{
                console.log("Something went wrong when submitting your transaction:", err);
                handleErrorNotification(err);
              }
            })
          } else {
            console.log("Something went wrong when submitting your transaction:", err);
            handleErrorNotification(err);
          }
        });
      }
      catch(error){
        console.log("Something went wrong when submitting your transaction:", error);
        handleErrorNotification(error);
      }
    }
  }

  const returnNft = async () => {
    if (!isAuthenticated){
      handleNewNotification();
    }else{
      let userWalletAddress = Moralis.User.current().attributes.ethAddress;
      console.log('Returning NFT to admin wallet: '+adminWalletAddress);
      const nonce = await w3.eth.getTransactionCount(adminWalletAddress, 'latest');
      var rawTransaction = {
        "from":adminWalletAddress,
        "to":contractAddress,
        "nonce":nonce,
        "gas":500000,
        "maxPriorityFeePerGas":1999999987,
        "data":contract.methods.returnToken(userWalletAddress,adminWalletAddress,0).encodeABI()
      };
      console.log(rawTransaction);
      let signedTransaction = await w3.eth.accounts.signTransaction(rawTransaction,PRIVATE_KEY);
      try{
        await w3.eth.sendSignedTransaction(signedTransaction.rawTransaction, async function(err, hash) {
          if (!err) {
            console.log("The hash of your transaction is: ", hash, "\n");
            await w3.eth.getTransactionReceipt(hash,(err,receipt) => {
              if (!err){
                console.log("Got transaction receipt: \n"+receipt);
                handleReturnNotification(hash); 
              }else{
                console.log("Something went wrong when submitting your transaction:", err);
                handleErrorNotification(err);
              }
            })
          } else {
            console.log("Something went wrong when submitting your transaction:", err);
            handleErrorNotification(err);
          }
        });
      }
      catch(error){
        console.log("Something went wrong when submitting your transaction:", error);
        handleErrorNotification(error);
      }
    }
  }

  return (
    <>
    <div className="header">
      <div className="logo">
        <img src={logo} alt="logo" height="300px"/>
      </div>
      <div className="walletconnect">
        <ConnectButton />
      </div>
    </div>
     <div className="instructions">
      Renting an NFT or purchasing door access ?
     </div>
     <div className="useractions">
      <Button
      onClick={() => {rentNft()}}
      color="blue"
      text="Rent an NFT"
      theme="colored"
      type="button" 
      ></Button>
      <Button
      onClick={() => {mapDoorAccess()}}
      color="green"
      text="Buy door access"
      theme="colored"
      type="button" 
      />
      <Button
      onClick={() => {returnNft()}}
      color="yellow"
      text="Return NFT"
      theme="colored"
      type="button" 
      />
     </div>
    </>
  );
};

export default App;