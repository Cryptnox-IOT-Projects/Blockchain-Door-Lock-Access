// SPDX-License-Identifier: GPL-3.0

pragma solidity >=0.7.0 <0.9.0;

import "./ERC721Rentable.sol";
import "@openzeppelin/contracts/access/Ownable.sol";

/**
 * @title SampleERC721
 * @dev Create a sample ERC721 standard token
 */
contract NFTDoorLock is Ownable,ERC721Rentable("DoorLock","DL") {

    uint tokenId;
    mapping(address=>tokenMetaData[]) public ownershipRecord;

    mapping(uint256 => address) public mappedOwners;

    struct tokenMetaData{
    uint tokenId;
    uint timeStamp;
    string tokenURI;
    }

    function mintToken(address recipient) onlyOwner public {
    _safeMint(recipient, tokenId);
    ownershipRecord[recipient].push(tokenMetaData(tokenId, block.timestamp, "https://ibb.co/cFsJP1L"));
    mappedOwners[tokenId] = recipient;
    tokenId = tokenId + 1; 
    }

    function returnToken(address from,address to,uint256 tokenToReturn) onlyOwner public{
        returnFrom(from,to,tokenToReturn);
    }

    function mapTokenToOwner(uint256 tokenToMap,address owner) onlyOwner public{
        require(_exists(tokenId), "Token doesn't exists.");
        mappedOwners[tokenToMap] = owner;
    }

    function checkOwner(uint256 tokenToCheck) public view returns (address owner){
        require(_exists(tokenId), "Token doesn't exists.");
        return mappedOwners[tokenToCheck];
    }


}