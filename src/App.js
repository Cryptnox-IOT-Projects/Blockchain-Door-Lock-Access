import React from "react";
import "./App.css";
import { ConnectButton, Button, useNotification } from "web3uikit";
import abi from "./dappLockAbi.json";
import { useMoralis, useWeb3ExecuteFunction } from "react-moralis";
import Elipses from "./components/homepage/ellipses";
import MainHeader from "./components/homepage/header";

const App = () => {

  const { fetch, isFetching } = useWeb3ExecuteFunction();
  const dispatch = useNotification();
  const { isAuthenticated, Moralis } = useMoralis();
  const contractAddress = "0xd529A426754A4DBDdAadB428c076B0C733175302";

  const handleNewNotification = () => {
    dispatch({
      type: "error",
      message: "Please connect  your crypto wallet",
      title: "Not Authenticated",
      position: "topL"
    })
  }

  const handleRentNotification = (txhash) => {
    dispatch({
      type: "success",
      message: "Token has been rented.\nTx hash:\n" + txhash,
      title: "Token rented",
      position: "topL"
    })
  }

  const handleErrorNotification = (error) => {
    dispatch({
      type: "error",
      message: "Something went wrong:\n" + error,
      title: "Unexpected error",
      position: "topL"
    })
  }

  const rentKeyToken = async (tokenId) => {
    if (!isAuthenticated) {
      handleNewNotification();
    }
    else {
      let options = {
        abi: abi,
        contractAddress: contractAddress,
        functionName: "rentToken",
        msgValue: Moralis.Units.ETH(0.001),
        params: {
          tokenIdForRent: tokenId,
        },
      };
      await fetch({
        params: options,
        onSuccess: (hash) => {
          console.log("Renting token, mining block...\n", hash);
          // handleRentNotification(hash);
        },
        onComplete: (hash) => {
          console.log("Rent completed.")
          handleRentNotification(hash);
        },
        onError: (err) => {
          console.log("Rent error, find error:\n" + err);
          handleErrorNotification(err);
        },
      });
    }
  }

  return (
    <>
      <div className="App">
        <Elipses />
        <div className="body">
          <MainHeader />
          <div className="header">
            <div className="walletconnect">
              <ConnectButton />
            </div>
          </div>
          <div className="instructions">
            Renting a key token for door access ?
          </div>
          <div className="rent-info">
            Contract address: {contractAddress}<br></br>
            Chain ID: 5<br></br>
            Chain Name: Görli<br></br>
            Currency: Görli ETH<br></br>
            Rent fees: 0.001 for each Token<br></br>
          </div>
          <div className="useractions">
            <Button
              onClick={() => { rentKeyToken(1) }}
              disabled={isFetching}
              color="blue"
              text="Rent Pi key token"
              theme="colored"
              type="button"
              className="cryptnox-button"
            ></Button>
            <div className="gap"></div>
            <Button
              onClick={() => { rentKeyToken(2) }}
              disabled={isFetching}
              color="green"
              text="Rent ESP key token"
              theme="colored"
              type="button"
              className="cryptnox-button"
            />
          </div>
        </div>
      </div>
    </>
  );
};

export default App;