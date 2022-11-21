import logo from '../../images/cryptnox-logo.png'

export default function MainHeader({handleUserSearch}) {
    return (
        <div>
            <div className="header-top">
                <img src={logo} className='logo' alt='logo' />
            </div>
            <div className="header-bot">
                
            </div>
        </div>
    );
}